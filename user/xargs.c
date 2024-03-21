#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void run(char *program, char **args){
    if(fork() == 0){  // child exec
        exec(program, args);
        exit(0);
    }
    return ;  //parent return
}
int main(int argc, char *argv[]){
    char buf[2048];  //读入时使用的内存池
    char *p = buf, *last_p = buf;  //读入一个参数时的开始指针和结束指针
    char *argsbuf[128];   //字符串指针数组，存放全部参数，包括argv传入的参数和stdin读入的参数
    char **args = argsbuf;  //用来指向argsbuf中第一个从stdin读入的参数
    for(int i = 1; i < argc; i++){
        // 将 argv 提供的参数加入到最终的参数列表中
        *args = argv[i];
        args++;
    }
    char **pa = args;
    //读取命令行输入的参数
    while(read(0, p, 1) != 0){
        if(*p == ' ' || *p == '\n'){
            //读入一个参数完成，以空格分隔
            *p = '\0'; // 将空格替换为 \0 分割开各个参数，这样可以直接使用内存池中的字符串作为参数字符串
            *(pa++) = last_p;  //将读到的参数存入argsbuf中
            last_p = p + 1;

            //读入一行完成
            if(*p == '\n'){
                *pa = 0;  //参数列表末尾用null标识结束
                run(argv[1], argsbuf);
                pa = args;  //重置读入参数指针， 准备读入下一行
            }
        }
        p++;
    }
    //处理最后一个参数
    if(pa != args){
        *p = '\0';
        *(pa++) = last_p;
        *pa = 0;
        run(argv[1], argsbuf);
    }
    while(wait(0) != -1){};  //循环等待所有子进程完成，每次wait(0)等待一个
    exit(0);
}