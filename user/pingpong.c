#include "kernel/types.h"
#include "user/user.h"

#define R 0
#define W 1

#define STDDER_FILENO 2

int main (int argc,char const *argv[]) {
    int p2c[2], c2p[2];
    pipe(p2c);
    pipe(c2p);

    if (fork() != 0) { //parent 
        char buf[2];
        // 父进程向子进程发送数据
        if (write (p2c[W],"!",1) != 1) {
            fprintf(STDDER_FILENO,"failed to write in parent");
            exit(1);
        }
        close(p2c[W]);
        //等待子进程执行完毕
        wait(0);

        // 父进程发送完毕后 等待 子进程的回复
        if (read(c2p[R],buf,1) != 1) {
            fprintf(STDDER_FILENO,"failed to read in parent");
            exit(1);
        }
        // 收到数据 输出pong
        printf("%d: received pong\n",getpid());
        close(c2p[R]);
    }else {
        char buf[2];
        // 子进程 读取管道 收到父进程的数据
        if (read(p2c[R],buf,1) != 1) {
            fprintf(STDDER_FILENO,"failed to read in child\n");
            exit(1);
        }
        close(p2c[R]);
        printf("%d: received ping\n",getpid());
        //子进程 通过 管道 把数据送回父进程
        if (write(c2p[W],buf,1) != 1) {
            fprintf(STDDER_FILENO,"failed to write in child\n");
            exit(1);
        }
        close(c2p[W]);
    }
    exit(0);

}