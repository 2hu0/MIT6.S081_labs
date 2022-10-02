#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define STDDER_FILENO 2

#define R 0
#define W 1
//每一次调用，从pleft中获取并且输出一个素数p,筛选p的所有倍数
//调用下一pipeline 的进程 以及相应输入管道pright，进行下一轮筛选
void primesSieve(int pleft[2])
{
    int prime, cur;
    //读第一个数
    if (read(pleft[R], &prime, sizeof(prime)) != sizeof(prime))
    {
        fprintf(STDDER_FILENO, "Child process failed to read\n");
        exit(1);
    }
    printf("prime %d\n", prime);

    if (read(pleft[R], &cur, sizeof(cur)))
    {
        int pright[2];
        pipe(pright); //创建用于输出到下一个pipeline的进程输出管道

        if (fork() == 0)
        {
            //下一个pipline
            close(pright[W]);
            close(pleft[R]);
            primesSieve(pright);
        }
        else
        {
            close(pright[R]);
            do
            {
                if (cur % prime != 0)
                { //筛去 prime的倍数,让其余的数字进入下一轮
                    write(pright[W], &cur, sizeof(cur));
                }
            } while (read(pleft[R], &cur, sizeof(cur)));

            close(pleft[R]);
            close(pright[W]);
            wait((int*)0);
        }
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    //主进程
    int fd[2];
    pipe(fd);
    if (fork() == 0)
    {
        //第一个pipe的子进程
        close(fd[W]);
        primesSieve(fd);
    }
    else
    {
        // parent
        close(fd[R]);
        for (int i = 2; i <= 35; i++)
        {
            if (write(fd[W], &i, sizeof(i)) != sizeof(i))
            {
                fprintf(STDDER_FILENO, "first process failed to write %d into the pipe!\n", i);
                exit(1);
            }
        }
        close(fd[W]);
        //等待子进程结束
        wait((int*)0);
    }
    exit(0);
}