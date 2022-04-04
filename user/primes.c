#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int fd) {
    int n;
    read(fd, &n, 4);
    printf("prime %d\n", n);

    int created = 0;
    int p[2];
    int num;
    while (read(fd, &num, 4) != 0) {
        // 读到了第二个数，且还没有创建过新进程，就创建一个新的
        if (created == 0) {
            pipe(p);
            created = 1;
            if (fork() == 0) {
                // child
                close(p[1]);
                // 子进程继续读取当前管道的数
                prime(p[0]);
            } else {
                close(p[0]);
            }
        }
        if (num % n != 0) {
            write(p[1], &num, 4);
        }      
    }
    close(fd);
    close(p[1]);
    wait(0);
}

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);

    if (fork() != 0) {
        // parent, 作为第一个传递所有数的进程
        close(p[0]);
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, 4);
        }
        close(p[1]);
        // 等待子进程执行结束
        wait(0);
    } else {
        close(p[1]);
        prime(p[0]);
        close(p[0]);
    }
    exit(0);
}