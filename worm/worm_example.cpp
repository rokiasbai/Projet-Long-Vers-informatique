//ScanH by Jhep
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h> // c:\dev-c++\lib\libwsock32.a

// Code en C++
// Vers qui scanne les adresses IP et/ou les ports
// Code brut, à modifier

int CompareIP(SOCKADDR_IN, SOCKADDR_IN);
void Use();
void End();
void Error();

SOCKET ws;

int main(int argc, char *argv[])
{

    if (argc < 5){
        Use();
        return 0;
    }

    if (!strcmp(argv[1], "-a")){

        SOCKADDR_IN from, to, ip;
        long Port;
        from.sin_family = to.sin_family = ip.sin_family = AF_INET;
        from.sin_addr.S_un.S_addr = inet_addr(argv[2]);
        to.sin_addr.S_un.S_addr = inet_addr(argv[3]);
        Port = atoi(argv[4]);

        if (from.sin_addr.S_un.S_addr ==  to.sin_addr.S_un.S_addr == INADDR_NONE ||
        CompareIP(from, to) == 1 || Port < 0 || Port > 65535) {
            Use();
            return 0;
        }

        WSADATA Info;
        if (WSAStartup(MAKEWORD(2,2),&Info)) {
            Error();
            return 0;
        };

        if ((ws = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
            Error();
            return 0;
        }

        printf(" ScanH\n\nScan les IP de %s à %s sur le port %s.\n\n", argv[2], argv[3], argv[4]);

        ip = from;
        ip.sin_port = htons(Port);
        do{
            printf("IP : %d.%d.%d.%d", ip.sin_addr.S_un.S_un_b.s_b1, ip.sin_addr.S_un.S_un_b.s_b2, ip.sin_addr.S_un.S_un_b.s_b3,
            ip.sin_addr.S_un.S_un_b.s_b4);

            if (!connect(ws, (sockaddr *) &ip, sizeof(ip))){
            printf(" OK");
            closesocket(ws);
            if ((ws = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {Error(); return 0;}
            }
            printf("\n");

            ip.sin_addr.S_un.S_un_b.s_b4++;
            if (ip.sin_addr.S_un.S_un_b.s_b4 > 255){
                ip.sin_addr.S_un.S_un_b.s_b4 = 0;
                ip.sin_addr.S_un.S_un_b.s_b3++;
            }
            if (ip.sin_addr.S_un.S_un_b.s_b3 > 255){
                ip.sin_addr.S_un.S_un_b.s_b3 = 0;
                ip.sin_addr.S_un.S_un_b.s_b2++;
            }
            if (ip.sin_addr.S_un.S_un_b.s_b2 > 255){
                ip.sin_addr.S_un.S_un_b.s_b2 = 0;
                ip.sin_addr.S_un.S_un_b.s_b1++;
            }
            if (ip.sin_addr.S_un.S_un_b.s_b1 > 255){
                break;
            }
        } while (CompareIP(ip, to) == 2 || CompareIP(ip, to) == 3);
        End();
        return 0;
    }

    if (!strcmp(argv[1], "-p")){

        SOCKADDR_IN ip;
        long Port1, Port2, Port;
        ip.sin_family = AF_INET;
        ip.sin_addr.S_un.S_addr = inet_addr(argv[2]);
        Port1 = atoi(argv[3]);
        Port2 = atoi(argv[4]);
        if (ip.sin_addr.S_un.S_addr == INADDR_NONE || Port1 < 0 || Port1 > 65535 || Port2 < 0 || Port2 > 65535 || Port1 > Port2) {
            Use();
        }

        WSADATA Info;
        WSAStartup(MAKEWORD(2, 2), &Info);
        if ((ws = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
            Error();
            return 0;
        }

        printf(" ScanH\n\nScan l'IP %s sur les ports %s à %s.\n\n", argv[2], argv[3], argv[4]);

        Port = Port1;
        do{
            printf("Port : %d", Port);
            ip.sin_port = htons(Port);
            if (!connect(ws, (sockaddr *) &ip, sizeof(ip))){
                printf(" OK", Port);
                closesocket(ws);
                if ((ws = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
                    Error();
                    return 0;
                }
            }
            printf("\n");
            Port++;
        } while(Port <= Port2);
        End();
    }
    else{
        Use();
    }

    return 0;
}

int CompareIP(SOCKADDR_IN IP1, SOCKADDR_IN IP2)
{
    double Nb1, Nb2;
    Nb1 = IP1.sin_addr.S_un.S_un_b.s_b4 + IP1.sin_addr.S_un.S_un_b.s_b3 * 0x100 + IP1.sin_addr.S_un.S_un_b.s_b2 * 0x10000 + IP1.sin_addr.S_un.S_un_b.s_b1 * 0x1000000;
    Nb2 = IP2.sin_addr.S_un.S_un_b.s_b4 + IP2.sin_addr.S_un.S_un_b.s_b3 * 0x100 + IP2.sin_addr.S_un.S_un_b.s_b2 * 0x10000 + IP2.sin_addr.S_un.S_un_b.s_b1 * 0x1000000;
    if (Nb1 > Nb2) return 1;
    if (Nb1 < Nb2) return 2;
    if (Nb1 == Nb2) return 3;
}

void Use()
{
    printf(" ScanH\n\nUtilisation :\nScanH.exe -a IPdeDepart IPdeFin Port\nou\nScanH.exe -p IP PortdeDepart PortdeFin\n");
    system("pause");
}

void End()
{
    closesocket(ws);
    WSACleanup();
    system("pause");
}

void Error()
{
    printf("\nErreur !\n");
    system("pause");
}