#include "server.h" 

void overflow(char *str, int n){
	char buffer[100];
	//strcpy(buffer, str); // Obvious Buffer Overflow
	memcpy(buffer, str, n);
}

// Function designed to listen to incoming messages on port 8080.
void chat_client(int client_socket){ //Traitée par le processus fils
	char buff[MAX];
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		int n = read(client_socket, buff, sizeof(buff));
		// print buffer which contains the client contents
		printf("From client: %s\n", buff);
		// Implement vulnerability

		overflow(buff, n);
		printf("Oui_1\n");
		printf("Oui_2\n");
		printf("Oui_3\n");
		write(client_socket, buff, sizeof(buff));
		// if msg is "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Child PID = %d : Server Exit...\n",getpid());
			break;
		}
	}
}

int cnt = 0;

int main()
{
	// Server socket id
	int sockfd, ret;

	// Server socket address structures
	struct sockaddr_in serverAddr;

	// Client socket id
	int clientSocket;

	// Client socket address structures
	struct sockaddr_in cliAddr;

	// Stores byte size of server socket address
	socklen_t addr_size;

	// Child process id
	pid_t childpid;

	// Creates a TCP socket id from IPV4 family
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// Error handling if socket id is not valid
	if (sockfd < 0) {
		printf("Error in connection.\n");
		exit(1);
	}

	printf("Server Socket is created.\n");

	// Initializing address structure with NULL
	memset(&serverAddr, '\0',
		sizeof(serverAddr));

	// Assign port number and IP address
	// to the socket created
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);

	// 127.0.0.1 is a loopback address
	serverAddr.sin_addr.s_addr
		= inet_addr("127.0.0.1");

	// Binding the socket id with
	// the socket structure
	ret = bind(sockfd,
			(struct sockaddr*)&serverAddr,
			sizeof(serverAddr));

	// Error handling
	if (ret < 0) {
		printf("Error in binding.\n");
		exit(1);
	}

	// Listening for connections (upto 10)
	if (listen(sockfd, 10) == 0) {
		printf("Listening...\n\n");
	}

	while (1) {

		// Accept clients and
		// store their information in cliAddr
		clientSocket = accept(
			sockfd, (struct sockaddr*)&cliAddr,
			&addr_size);

		// Error handling
		if (clientSocket == -1) {
			exit(1);
		}

		// Displaying information of connected client
		printf("Connection accepted from %s:%d\n",
			inet_ntoa(cliAddr.sin_addr),
			ntohs(cliAddr.sin_port));

		// Print number of clients connected till now
		cnt++;
		printf("Client session %d\n\n",cnt);

        //Parent PID
        printf("Parent server PID = %d\n", getpid());


		// Creates a child process
		if ((childpid = fork()) == 0) {
            printf("Fork successful\nChild PID = %d\n", getpid());
			// Closing the server socket id
			close(sockfd);
            chat_client(clientSocket);
			printf("Closing client session %d\n",cnt); 
            close(clientSocket);
		}
	}

	// Close the client socket id
	close(clientSocket);
	return 0;
}
