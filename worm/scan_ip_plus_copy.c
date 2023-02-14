#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define MAX_IP_LENGTH 15
#define SCRIPT_PATH "/home/sparrow/Desktop/Rokia/script.sh"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s start_ip end_ip\n", argv[0]);
        return 1;
    }

    char start_ip[MAX_IP_LENGTH+1];
    char end_ip[MAX_IP_LENGTH+1];
    strncpy(start_ip, argv[1], MAX_IP_LENGTH);
    strncpy(end_ip, argv[2], MAX_IP_LENGTH);

    // Convert start and end IP addresses to integers
    uint32_t start_addr = ntohl(inet_addr(start_ip));
    uint32_t end_addr = ntohl(inet_addr(end_ip));

    // Loop through IP addresses
    for (uint32_t i = start_addr; i <= end_addr; i++) {
        struct in_addr addr;
        addr.s_addr = htonl(i);
        char ip_str[MAX_IP_LENGTH+1];
        strncpy(ip_str, inet_ntoa(addr), MAX_IP_LENGTH);

        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            char command[1024];
            snprintf(command, sizeof(command), "scp %s sparrow@%s:/home/sparrow/Desktop", SCRIPT_PATH, ip_str);
            setuid(getuid());
            system(command);
            exit(0);
        } else if (pid < 0) {
            perror("fork");
        }
    }

    // Wait for all child processes to finish
    while (wait(NULL) > 0) {}

    return 0;
}
