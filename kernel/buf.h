struct buf {
  int valid;   // has data been read from disk?含磁盘块数据
  int disk;    // does disk "own" buf?缓存区数据已被提交到磁盘
  uint dev;    //设备号
  uint blockno;   //缓存数据块号
  struct sleeplock lock;
  uint refcnt;      //被引用次数
  struct buf *prev; // LRU cache list
  struct buf *next;
  uchar data[BSIZE];
};

