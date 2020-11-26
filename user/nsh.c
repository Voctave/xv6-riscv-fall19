#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
  
#define MAXARGS 10
#define MAXWORD 30
#define MAXLINE 100
 
void execPipe(char*argv[],int argc);
//读取命令
int getcmd(char *buf, int nbuf);
void setargs(char *cmd, char* argv[],int* argc);
//重定向&执行命令
void runcmd(char*argv[],int argc);

char whitespace[] = " \t\r\n\v";
char args[MAXARGS][MAXWORD]; 

int main()
{
    char buf[MAXLINE];
    // Read and run input commands.
    while (getcmd(buf, sizeof(buf)) >= 0)
    {
        if (fork() == 0)
        {
            char* argv[MAXARGS];
            int argc=-1;
            setargs(buf, argv,&argc);
            runcmd(argv,argc);
        }
        wait(0);
    }
    exit(0);
}

int getcmd(char *buf, int nbuf)
{
    fprintf(2, "@ ");
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if (buf[0] == 0) // EOF
        return -1;
    return 0;
}

void execPipe(char*argv[],int argc){
    int i=0;
    // 将命令中的"|"换成'\0'
    for(i=0;i<argc;i++){
        if(!strcmp(argv[i],"|")){
            argv[i]="\0";
            break;
        }
    }
    // 最简单的情况：cat file | wc
    int fd[2];
    pipe(fd);
    if(fork()==0)
    {
        // 子进程 执行左边的命令,关闭标准输出
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argv,i);
    }
    else
    {
        // 父进程 执行右边的命令，关闭标准输入
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argv+i+1,argc-i-1);
    }
}

void setargs(char *cmd, char* argv[],int* argc)
{
    // 让argv的每一个元素都指向args的每一行
    for(int i=0;i<MAXARGS;i++){
        argv[i]=&args[i][0];
    }
    int i = 0; // 表示第i个word
    int j = 0;
    for (; cmd[j] != '\n' && cmd[j] != '\0'; j++)
    {
        // 每一轮循环都是找到输入的命令中的一个word，比如 echo hi ,就是先找到echo，再找到hi
        // 让argv[i]分别指向他们的开头，并且将echo，hi后面的空格设为\0
        // 跳过之前的空格符号
        while (strchr(whitespace,cmd[j])){
            j++;
        }
        argv[i++]=cmd+j;
        // 只要不是空格符号，就j++,找到下一个空格符号
        while (strchr(whitespace,cmd[j])==0){
            j++;
        }
        cmd[j]='\0';
    }
    argv[i]=0;
    *argc=i;
}

void runcmd(char*argv[],int argc)
{
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"|")){//相等返回0
            // 如果遇到 | 即pipe，说明后面还有命令要执行
            execPipe(argv,argc);
        }
    }
    // 仅处理一个命令：现在判断argv[1]开始，后面有没有> 
    for(int i=1;i<argc;i++){
        // 如果遇到 > ，说明需要执行输出重定向，需要关闭stdout
        if(!strcmp(argv[i],">")){
            close(1);
            // 把输出重定向到文件名对应的文件
            open(argv[i+1],O_CREATE|O_WRONLY);
            argv[i]=0;
        }
        if(!strcmp(argv[i],"<")){
            // 如果遇到< ,需要执行输入重定向，关闭stdin
            close(0);
            open(argv[i+1],O_RDONLY);
            argv[i]=0;
        }
    }
    exec(argv[0], argv);
}