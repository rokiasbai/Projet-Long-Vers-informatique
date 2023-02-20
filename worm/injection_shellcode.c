#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> // inet_addr()
#include <sys/socket.h>
#include <unistd.h> 
#define MAX 80

// Voir fonction strcat pour concaténer des chars (string.h)

void func(int sockfd)
{
	char buff[MAX];
	int n;
	for (;;) {
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}
}


int main (int argc, char *argv[]){

    int a = 0;
    int b = 2;
/*
    if (argc != 1) {
        printf("Erreur: la fonction %s ne requiert aucun parametres\n", argv[0]);
        return 1;
    }


    
    // Ecriture en dur du shellcode : 
    // NOP *40(etc 1 octet chaque) + shellcode (taille à définir POUR TAILLE BOF) 
    // + "A" *4(1 octet chaque) + @retour_dans_nop (== @relatif A CHERCHER GDB BASE PILE - [size(shellcode) + 4* A + moitié Nop])
    char shellcode[] = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x42\x41\x41\x41\x41\x40"; // A COMPLETER

    // COMMENT @retour = @base pile + taille instruction ENTIERE !!!


    // function for chat
	// func(sockfd);


    printf("OK\n");
    printf("%s\n", shellcode);
    return 0;*/
}