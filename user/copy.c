//将控制台的输入内容输出到控制台

#include "kernel/types.h"
#include "user/user.h"

int 
main()
{
    char buf[64];
    int n;
    while((n = read(0, &buf, sizeof(buf))) > 0){
        if(write(1, &buf, n) != n){
            fprintf(2, "write error\n");
            exit(1);
        }
        
    }
    if(n < 0){
        fprintf(2, "read error\n");
        exit(1);
    }
        
    exit(0);

}