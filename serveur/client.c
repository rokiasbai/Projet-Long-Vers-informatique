#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

extern char __start_worm;
extern char __stop_worm;
__attribute((__section__("worm")))


void func(int sockfd)
{
	/*char s[4];
	s[0] = 'A';
	s[1] = 'A';
	s[2] = 'A';
	s[3] = 'A';
	write(1, s, 4);
	*/
// CODER EN DUR LE SHELLCODE
// AFFCIHER QQCHOSE COTE SERVEUR
asm("push %rsi\n\t"
	"lea (%rip),%rsi\n\t"
	"jmp 1f\n\t"
	".byte 65\n\t"
	".byte 69\n\t"
	".byte 65\n\t"
	".byte 65\n\t"
	"1:\n\t"
	"add $2,%rsi\n\t"
    "xor %rax, %rax\n\t"
    "mov $1, %al\n\t"
    "mov %rax, %rdi\n\t"
    "mov %rdi, %rdx\n\t"
    "add $5, %rdx\n\t"
    "syscall\n\t"
	"pop %rsi\n\t");
/*    "code:
    pop rsi;
    xor rax, rax;
    mov al, 1;
    mov rdi, rax;
    mov rdx, rdi;
    add rdx, 14;
    syscall;

    xor rax, rax;
    add rax, 60;
    xor rdi, rdi;
    syscall;
	jmp short cont;

    string:
    call code;
    db  'Hello, world!',0x0A;
cont:
");*/

	char buff[MAX];
	int n;
	for (;;) {
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;

		char *s = &__start_worm;

		for ( ; s < &__stop_worm; ++s, n++) {
			buff[n] = *s;
		}

		for (int i = 0; i < n; i++) {
			printf(" %02x", 0xff & buff[i]);
			if (i % 16 == 0) {
				printf("\n");
			}
		}
		printf("%p  %p  %p\n", func, &__start_worm, &__stop_worm);

		//while ((buff[n++] = getchar()) != '\n')
		//	;

		write(sockfd, buff,n);
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
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

