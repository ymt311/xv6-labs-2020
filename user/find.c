#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

//从路径中获取文件名
char* fmt_name(char *path){  
    static char buf[DIRSIZ + 1];
    char *p;
    //找到最后一个'/'后的第一个字符，就是文件名的起点
    for(p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    memmove(buf, p, strlen(p) + 1);
    return buf;
}

//判断文件名是否跟指定的文件名相同
void eq_print(char *fileName, char *findName){
    if(strcmp(fmt_name(fileName), findName) == 0){
        printf("%s\n", fileName);
    }
}

void find(char *path, char *findName){
    int fd;
    struct stat st;

    //打开传入的文件
    if((fd = open(path, 0)) < 0){
		fprintf(2, "find: cannot open %s\n", path);
		exit(1);
	}
    //获取文件状态
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		exit(1);
    }

    char buf[512], *p;
    struct dirent de;
    switch(st.type){
        case T_FILE:
            eq_print(path, findName);
            break;
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
				printf("find: path too long\n");
				break;
			}
            strcpy(buf, path);
            p = buf + strlen(buf); //p指向buf中最后一个字符的下一个位置'\0'
            *p++ = '/';  //将'\0'替换为'/'，再让p指向下一个位置
            //读目录项
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                // inum 通常代表文件的索引号或者 inode 号
                // 索引号为 0 的文件通常是特殊文件或者保留给根目录的，索引号为 1 的文件通常是表示当前目录（.）的目录项
                if(de.inum == 0 || de.inum == 1 || strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0)
					continue;
                memmove(p, de.name, strlen(de.name));
                p[strlen(de.name)] = 0;
                find(buf, findName);
            }
            break;
    }
    close(fd);
}
int main(int argc, char *argv[]){
    if(argc < 3){
        printf("find: find <path> <fileName>\n");
		exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
    
    
