#include "kernel/types.h"
#include "user/user.h"

void Sift(int* number, int num);

void main(int argc, char* argv[])
{
    int number[34];
    int i;
    for(i=0;i<34;i++)
    {
        number[i]=i+2;
    }
    Sift(number,34);
    return;
}

void Sift(int* number, int num)
{
    int fd[2];
    int i;
    int div;//chosen prime number
    int new_num=0;
    char buf[4];//缓冲区到底怎么初始化！
    int temp;
    if(num==1)
    {
        printf("prime %d\n",number[0]);
        return;
    }
    div=number[0];
    printf("prime %d\n",div);
    pipe(fd);
    if(fork()==0)
    {
        close(fd[0]);
        //fd[1]=dup(1);//确认dup的作用
        //printf("%d\n",num);
        for(i=0;i<num;i++)  //write numbers into pipe
        {
            write(fd[1],(char*)(number+i),2);//如何判断字节
            //test
            //printf("%d\n",number[i]);
        }
        exit();
    }
    
    close(fd[1]);
    //fd[0]=dup(0);
    while(read(fd[0],buf,2)!=0)
    {
        temp=*(int*)buf;
        if(temp%div!=0)
        {
            number[new_num]=temp;
            new_num++;
            //test
            //printf("%d\n",number[new_num]);
        }
    }
    Sift(number,new_num);
    wait();
    exit();
    return;
}