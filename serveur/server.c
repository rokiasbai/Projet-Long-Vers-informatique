#include "server.h"

void overflow(char *str){
	char buffer[5];
	strcpy(buffer, str); // Obvious Buffer Overflow
	printf("message = %s", buffer);
}

// Function designed to listen to incoming messages on port 8080.
void listen_on_port(int connfd){
	char buff[MAX];
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(connfd, buff, sizeof(buff));
		// print buffer which contains the client contents
		printf("From client: %s\n", buff);
		//bzero(buff, MAX);
		//n = 0;
		// copy server message in the buffer
		//while ((buff[n++] = getchar()) != '\n')
		// and send that buffer to client
		overflow(buff);
		write(connfd, buff, sizeof(buff));
		//printf("Sending to client buff = %s\n", buff);
		// if msg is "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}

// Driver function
int main()
{
	int sockfd, connfd, connfd_bis, len;
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
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed... error : %s\n", strerror(errno));
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);
	
	// Accept the data packet from client, verification and fork
	if(connfd_bis = accept(sockfd,(SA*)&cli,&len) < 0){
		printf("Server accept failed\n");
		exit(0);
	}
	else{
		printf("Server accepted client\n");
	}

	switch(fork()){
		case -1:
			printf("Server fork failed");
			exit(-1);
		case 0: // Proc fils
			close(connfd);
			listen_on_port(connfd_bis);
			close(connfd_bis);
		default: //Dans proc père, close sock fils par defaut
			close(connfd_bis);
	}
}

