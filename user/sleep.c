#include "kernel/types.h"
#include "user/user.h"

void main(int argc, char* argv[])
{
    int num;
    if(argc<2)
    {
        printf("Lack of parameter!\n");
        exit();
    }
    else if(argc>2)
    {
        printf("Too many parameters!\n");
        exit();
    }
    num=atoi(argv[1]);
    printf("(Nothing happens for a little while)\n");
    sleep(num);
    exit();
}