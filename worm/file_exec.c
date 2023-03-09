#include <sys/types.h>
#include <sys/stat.h>
#include <asm/unistd.h>      // compile without -m32 for 64 bit call numbers
#define __NR_write 1
#define O_CREAT 00000100
#define O_WRONLY 00000001
#define O_RDWR 00000002
#define O_TRUNC 00001000
#define __NR_open 2
#define __NR_close 3
#define __NR_execve 59


ssize_t my_write(int fd, const void *buf, size_t size);
int my_open(const char * name, int flags, mode_t mode);
int my_close(int fd);
int my_execve(const char *fichier, char *const argv[], char *const envp[]);

int main(void){
    const char hello[] = "#!/bin/bash\npwd\n";
    int fd = my_open("/tmp/maybe_a_virus", O_CREAT | O_RDWR | O_TRUNC, 00700);
    my_write(fd, hello, sizeof(hello));
    my_close(fd);
    char buff[100];
    my_execve("/tmp/maybe_a_virus",0,0);
    char * s = &my_write;
    for (int i =0;s<&my_open;s++,i++){
        buff[i]=*s;
        printf(" %02x",0xff& buff[i]);
        if (i%16 ==0){
            printf("\n");
        }
    }
    printf("\n");    
    return 0;
}


ssize_t my_write(int fd, const void *buf, size_t size){
    ssize_t ret;
    asm volatile
    (
        "syscall"
        : "=a" (ret)
        //                 EDI      RSI       RDX
        : "0"(__NR_write), "D"(fd), "S"(buf), "d"(size)
        : "rcx", "r11", "memory"
    );
    return ret;
}


int my_open(const char * name, int flags, mode_t mode){
    int fd;
    asm volatile
    (
        "syscall"
        : "=a" (fd)
        : "0" (__NR_open), "D" (name), "S" (flags), "d" (mode)
        : "rcx", "r11", "memory"
    );
    return fd;
}

int my_close(int fd){
    int ret;
    asm volatile
    (
        "syscall"
        :"=a" (ret)
        : "0" (__NR_close), "D" (fd)
        : "rcx", "r11", "memory"
    );
    return ret;
}

int my_execve(const char *name, char *const argv[], char *const envp[]){
    int ret;
    asm volatile
    (
        "syscall"
        :"=a" (ret)
        : "0" (__NR_execve), "D" (name),"S" (argv), "d" (envp)
        : "rcx", "r11", "memory"
    );
    return ret;
}
