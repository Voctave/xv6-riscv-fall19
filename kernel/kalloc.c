// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
//#include "unistd.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct kmem{
  struct spinlock lock;
  struct run *freelist;
};
struct kmem kmems[NCPU];//

void
kinit()
{
  initlock(&kmems[0].lock, "kmem0");
  initlock(&kmems[1].lock, "kmem1");
  initlock(&kmems[2].lock, "kmem2");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);//将可用内存分片
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);//在kfree中将片加入空闲链表
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  int id=0;
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  push_off();
  id=cpuid();;//获取当前cpu序号号
  pop_off();
  acquire(&kmems[id].lock);
  r->next = kmems[id].freelist;
  kmems[id].freelist = r;
  release(&kmems[id].lock);
}


// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  int id;
  struct run *r;
  int i;
  push_off();
  id=cpuid();//获取当前cpu序号号
  pop_off();
  acquire(&kmems[id].lock);
  r = kmems[id].freelist;
  if(r)
    kmems[id].freelist = r->next;
  release(&kmems[id].lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  else
  {
    for(i=0;i<3;i++)
    {
      if(i==id)
      {
        continue;
      }
      acquire(&kmems[i].lock);
      r=kmems[i].freelist;
      if(r)
      {
        kmems[i].freelist = r->next;
      }
      release(&kmems[i].lock);
      if(r)
      {
        memset((char*)r, 5, PGSIZE); // fill with junk
        break;
      }
    }
  }
  
  return (void*)r;
}
