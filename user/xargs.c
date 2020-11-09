#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int i;
    int j = 0;
    int k;
    int l,m = 0;
    char block[32];
    char buf[32];
    char *p = buf;
    char *lineSplit[32];
    for(i = 1; i < argc; i++){
        lineSplit[j++] = argv[i];//第一行参数
    }
    while( (k = read(0, block, sizeof(block))) > 0){//cat 继续读下面的参数，辨别换行
        for(l = 0; l < k; l++){
            if(block[l] == '\n'){
                buf[m] = 0;
                m = 0;
                lineSplit[j++] = p;
                p = buf;
                lineSplit[j] = 0;
                j = argc - 1;//覆盖之前的参数，继续执行
                if(fork() == 0){
                    exec(argv[1], lineSplit);//
                }                
                wait();
            }else if(block[l] == ' ') {
                buf[m++] = 0;//为什么赋0，命令的结束
                lineSplit[j++] = p;//linesplit换行继续读入，buffer继续记录
                p = &buf[m];
            }else {
                buf[m++] = block[l];
            }
        }
    }
    exit();
}