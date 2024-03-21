#include "kernel/types.h"
#include "user/user.h"

int
main()
{
    int p1[2];   //写端父进程，读端子进程
    int p2[2];  //写端子进程， 读端父进程
    int ret, pid;

    ret = pipe(p1);
    if(ret == -1){
        fprintf(2, "pipe 1 error\n");
        exit(1);
    }
    ret = pipe(p2);
    if(ret == -1){
        fprintf(2, "pipe 2 error\n");
        exit(1);
    }

    pid = fork();
    if(pid < 0){
        fprintf(2, "fork error\n");
        exit(1); 
    }else if(pid > 0){  //父进程
        close(p1[0]);  //关闭父进程管道1 读端
        close(p2[1]);  //关闭父进程管道2 写端
        char buf;

        //父进程向管道1中写入一个字节的数据
        if(write(p1[1], "a", 1) != 1){  
            fprintf(2, "parent write error\n");
            exit(1); 
        }
        //父进程从管道2中读入一个字节的数据
        if(read(p2[0], &buf, sizeof(buf)) != 1){
            fprintf(2, "parent read error\n");
            exit(1); 
        }
        printf("%d: received pong\n", getpid());

        //等待子进程退出，回收
        wait(0);
        exit(0);
    }else{  //pid == 0，子进程
        close(p1[1]);  //关闭子进程管道1 写端
        close(p2[0]);  //关闭子进程管道2 读端
        char buf;

        //子进程从管道1中读入一个字节的数据
        if(read(p1[0], &buf, sizeof(buf)) != 1){
            fprintf(2, "child read error\n");
            exit(1); 
        }
        printf("%d: received ping\n", getpid());

        //子进程向管道2中写入一个字节的数据
        if(write(p2[1], &buf, 1) != 1){  
            fprintf(2, "child write error\n");
            exit(1); 
        }
        exit(0);
    }
}