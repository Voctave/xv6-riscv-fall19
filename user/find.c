#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* fmtname(char* path);
void Findfile(char* path, char* filename);

void main(int argc, char* argv[])
{
    if(argc<3)
    {
        printf("Please input <path> <filename>\n");
        exit();
    }
    Findfile(argv[1],argv[2]);
    exit();
}

//将路径格式化为文件名
char* fmtname(char* path)
{
    char *t=path;
    char *last=0;
    while(*t!='\0'){
        if(*t=='/'){
            last=t;
        }
        t++;
    }
    // 也可能没有/，那么p指向的文件名
    if(last==0){
        return path;
    }
    return last+1;

}

void Findfile(char* path, char* filename)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd=open(path,0))<0)
    {
        fprintf(2, "ls:cannot open %s\n", path);
        return;
    }
    if(fstat(fd,&st)<0)
    {
        fprintf(2, "ls:cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch(st.type)
    {
        case T_FILE:
        p=fmtname(path);
        if(strcmp(p,filename)==0)
        {
            printf("%s\n",path);
        }
        break;

        case T_DIR:
        if(strlen(path)+1+DIRSIZ+1 > sizeof buf)//strlen(path)+1+DIRSIZ+1意味着什么
        {
            printf("ls:path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd,&de,sizeof(de))==sizeof(de))//循环表示的意思？/////////////////////
        {
            if(de.inum ==0)//dirent无效
            {
                continue;
                
            }
            //计算当前dirent路径   ???
            int t = strlen(de.name)>DIRSIZ?DIRSIZ:strlen(de.name);
            memmove(p,de.name,t);//buff=path/de.name
            p[t]=0;
            //p让buff成为当前dirent的完整路径
            if(stat(buf, &st)<0)
            {
                printf("ls: cannot stat %s\n", buf);
                continue;
            
            }
            if(strcmp(de.name,".")==0||strcmp(de.name,"..")==0)
            {
                continue;

            }
            Findfile(buf,filename);
        }
        break;
    }
    close(fd);
}