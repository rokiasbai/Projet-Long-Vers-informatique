#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s start_ip end_ip\n", argv[0]);
        return 1;
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;

    struct in_addr start_ip, end_ip;
    if (inet_pton(AF_INET, argv[1], &start_ip) == 0 ||
        inet_pton(AF_INET, argv[2], &end_ip) == 0) {
        fprintf(stderr, "Invalid IP address\n");
        return 1;
    }

    unsigned int start_ip_val = ntohl(start_ip.s_addr);
    unsigned int end_ip_val = ntohl(end_ip.s_addr);

    for (unsigned int ip = start_ip_val; ip <= end_ip_val; ip++) {
        sa.sin_addr.s_addr = htonl(ip);
        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sa.sin_addr, addr_str, INET_ADDRSTRLEN);

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            perror("socket");
            return 1;
        }

        if (connect(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == 0) {
            printf("%s is up\n", addr_str);
            close(sockfd);
        }
    }

    return 0;
}

