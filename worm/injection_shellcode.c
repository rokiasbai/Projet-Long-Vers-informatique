#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main (int argc, char *argv[]){

    if (argc != 1) {
        printf("Erreur: la fonction %s ne requiert aucun parametres\n", argv[0]);
        return 1;
    }
    // Voir fonction strcat pour concaténer des chars
    char shellcode[] = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x42\x41\x41\x41\x41\x40"; // A COMPLETER


    // NOP *30(etc 1 octet chaque) + shellcode (taille à définir POUR TAILLE BOF) 
    // + "A" *4(1 octet chaque) + @retour_dans_nop (== relatif - [size(shellcode) + 4* A + moitié Nop])


    printf("OK\n");
    printf("%s\n", shellcode);
    return 0;
}