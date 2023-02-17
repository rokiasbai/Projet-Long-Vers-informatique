#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>


#define NB_TARGET 5
#define MAX 80
#define PORT 8080
#define SA struct sockaddr


//typedef struct cell_IPs cell_IPs;
struct cell_IPs {
    char adresse;
    struct cell_IPs *pSuivant;
};

//typedef struct liste liste;
struct liste {
    struct cell_IPs *Debut;
    struct cell_IPs *Position;
    struct cell_IPs *Fin;
};


#define IP_SIZE 15


int main(int argc, char *argv[]){

    if (argc != 3) {
        printf("Erreur: la fonction %s requiert les parametres start_ip et end_ip\n", argv[0]);
        return 1;
    }

    /*struct liste *Liste_Infecte;
    Liste_Infecte->Debut = NULL;
    Liste_Infecte->Position = Liste_Infecte->Debut;
    Liste_Infecte->Fin = NULL;*/

    /*struct IPs *Liste_Vuln;
    debut_Vuln = NULL;*/
    

    char start_ip[IP_SIZE+1];
    char end_ip[IP_SIZE+1];
    strncpy(start_ip, argv[1], IP_SIZE);
    strncpy(end_ip, argv[2], IP_SIZE);

    printf("Scan from adress %s to %s\n", start_ip, end_ip);

    // Convert start and end IP addresses to integers
    uint32_t start_addr = ntohl(inet_addr(start_ip));
    uint32_t end_addr = ntohl(inet_addr(end_ip));

    // if A FAIRE

    //printf("%u\n", start_addr);
    //printf("%u\n", end_addr);

    uint32_t j = start_addr;

    // MAIN Loop through IP addresses
    for (uint32_t i = start_addr; i <= end_addr; i++) {
        struct in_addr addr;
        addr.s_addr = htonl(i); // convert back CURRENT uint32_t to IP addr (mais non printable)
        char ip_str[IP_SIZE+1];
        
        strncpy(ip_str, inet_ntoa(addr), IP_SIZE); // inet_ntoa => addr 10.0.0... LISIBLE
        printf("%s\n", ip_str);
        

        // IF A COMPLETER : pour chaque adresse, vérif si elle existe sur le reseau => test connexion serveur ok
        // + test si vulnérable ?
        // + si tout est vérifié => ajout de l'adresse à la liste MAIN à créer


        int socket_ver;
        struct sockaddr_in servaddr;

        // socket create and verification
        socket_ver = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_ver == -1) {
            printf("socket creation failed...\n");
            exit(0);
        }
        else
            printf("Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(ip_str); // POURQUOI NE MARCHE QU'AVEC 127.0.0.... ???
        servaddr.sin_port = htons(PORT);
        printf("OK\n");


        // connect the client socket to server socket                       IMPORTANT ICI
        if (connect(socket_ver, (SA*)&servaddr, sizeof(servaddr))
            != 0) {
            printf("connection with the server failed...\n");
        }
        else
            printf("connected to the server..\n");

            /*****************************************************************************/
            // UNE FOIS ICI, on peut AUSSI balancer la charge / lignes hexa pour overflow 
            /*****************************************************************************/

            /*cell_IPs *new;
            if (Liste_Infecte->Debut == NULL){
                Liste_Infecte->Fin = malloc(sizeof(cell_IPs));
                printf("%u\n", i);
                printf("%s\n", ip_str);
                Liste_Infecte->Fin->adresse = *ip_str;
                Liste_Infecte->Fin->pSuivant = NULL;
                Liste_Infecte->Debut = Liste_Infecte->Fin;
                Liste_Infecte->Position = Liste_Infecte->Debut;
            }
            else {
                new = Liste_Infecte->Fin;
                Liste_Infecte->Fin = malloc(sizeof(cell_IPs));
                Liste_Infecte->Fin->adresse = *ip_str;
                Liste_Infecte->Fin->pSuivant = NULL;
                new->pSuivant = Liste_Infecte->Fin;
            }*/


        // close the socket
        close(socket_ver);



        // close() OU shutdown()
        
    }


    // PRINT IP IN LISTE HERE


    /*int infecte = 0;
    struct in_addr in;
    in.s_addr = 100;

    int i = 1;
    while(i > 0){
        for(int i=0;i<3;i++){
            printf("My unreadable addres is %lX\n", in.s_addr);
        }
        i--;
    }*/
}

