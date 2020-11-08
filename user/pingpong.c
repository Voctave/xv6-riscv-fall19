#include "kernel/types.h"
#include "user/user.h"

void main(int argc, char* argv[])
{
    int fd1[2];
    int fd2[2];
    char buf[]={'X'};   //如何初始化buf？why？
    int n1;
    int n2;

    //create a pipe, with two FDs in fds[0],fds[1].
    pipe(fd1);//创建管道函数
    pipe(fd2);
    
    if(fork()==0)    //child
    {
        close(fd1[1]);
        close(fd2[0]);//关掉不用的fd1[1]、fd2[0]
        n1 = read(fd1[0], buf, sizeof(buf));   //fd[0]读操作
        if(n1!=sizeof(buf))
        {
            printf("child read error!\n");
            exit();
        }
        printf("%d: received ping\n",getpid());
        write(fd2[1],"pong\n",6);       //最后一个参数如何判断？
        if(write(fd2[1],"pong\n",6)!=6)
        {
            printf("child write error!\n");
            exit();
        }
        exit();
    }
    //parent
    close(fd1[0]);
    close(fd2[1]);
    write(fd1[1],"ping\n",6);      //fd[1]写操作
    if(write(fd1[1],"ping\n",6)!=6)
    {
        printf("parent write error!\n");
        exit();
    }
    n2 = read(fd2[0],buf, sizeof(buf));
    if(n2!=sizeof(buf))
    {
        printf("parent read error!\n");
        exit();
    }
    printf("%d: received pong\n",getpid());
    wait();
    exit();
}