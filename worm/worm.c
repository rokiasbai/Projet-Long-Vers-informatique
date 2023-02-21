#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080

struct cell_server {
    unsigned long server_addr;
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
	p_cell->server_addr = address;

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
		printf("%022lx ----> ",aux->server_addr);
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



int main(int argc, char * argv[]) {
	// Check argc
	if (argc != 3){
		printf("Must give 2 arguments\nExample : %s <start_IP> <end_IP>\n",argv[0]);
		exit(-1);
	}
	/* Scan IP given for server listening on port 8080
    then puts them in a linked list*/
    scan_server_available(argv[1],argv[2]);
    return 0;
}
