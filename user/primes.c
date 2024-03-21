#include "kernel/types.h"
#include "user/user.h"

// 一次sieve调用是一个筛选阶段，会从pleft获取并输出一个素数p，并筛除掉p的所有倍数
// 同时创建下一阶段的进程以及相应的输入管道pright，将未被筛除的值传到管道pright中，供下一阶段处理
void
sieve(int pleft[2])
{
    int p;
    read(pleft[0], &p, sizeof(p));  //读输入管道中第一个数，必然是素数
    if(p == -1){  //如果读到-1，表示所有数字处理完毕，退出程序
        exit(0);
    }
    printf("prime %d\n", p);

    int pright[2];
    pipe(pright);

    if(fork() == 0){
        close(pleft[0]);   //孙子进程是从子进程的管道读取数据，与父进程的管道无关了
        close(pright[1]);
        sieve(pright);
    }else{
        //当前stage 将未被筛除的值写入管道pright
        close(pright[0]);
        int buf;
        while(read(pleft[0], &buf, sizeof(buf)) && buf != -1){
            if(buf % p != 0){
                write(pright[1], &buf, sizeof(buf));
            }
        }
        buf = -1;
        write(pright[1], &buf, sizeof(buf));  //补写最后的-1，作为结束标识
        wait(0); //等待当前stage的下一stage完成
        exit(0);
    }
}

int
main(int argc, char *argv[])
{
    int input_pipe[2];   //主进程用来输入数据的管道
    pipe(input_pipe);

    if(fork() == 0){
        close(input_pipe[1]);
        sieve(input_pipe);
        exit(0);
    }else{
        //主进程向输入管道中写数据
        close(input_pipe[0]);
        int i;
        for(int i = 2; i <= 35; i++){
            write(input_pipe[1], &i, sizeof(i));
        }
        i = -1;
        write(input_pipe[1], &i, sizeof(i));  //末尾输入-1，用于标识输入完成
    }
    wait(0);
    exit(0);
}