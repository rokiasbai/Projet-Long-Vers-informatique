#include <sys/types.h>
#include <sys/stat.h>
#include <asm/unistd.h>      // compile without -m32 for 64 bit call numbers
#define __NR_write 1
#define O_CREAT 00000100 
#define __NR_open 2
#define __NR_close 3


ssize_t my_write(unsigned int fd, const void *buf, size_t size);
int my_open(const char * name, int flags, mode_t mode);
int my_close(unsigned int fd);

int main(void){
    const char hello[] = "Nothing to see here\n";
    unsigned int fd = my_open("/tmp/maybe_a_virus", O_CREAT, 00700);
    my_write(1,hello, sizeof(hello));
    my_write(fd, hello, sizeof(hello));
    my_close(fd);
    return 0;
}


ssize_t my_write(unsigned int fd, const void *buf, size_t size){
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
    unsigned int fd;
    asm volatile
    (
        "syscall"
        : "=a" (fd)
        : "0" (__NR_open), "D" (name), "S" (flags), "d" (mode)
        : "rcx", "r11", "memory"
    );
    return fd;
}

int my_close(unsigned int fd){
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