#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    char message[1024];

    // 1. Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 2. Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Connected to chat server! Type your messages below:\n\n");

    // 3. Fork process to handle simultaneous reading and writing
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child Process: Continuously reads incoming messages from the server
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int valread = read(sock, buffer, 1024);
            if (valread <= 0) {
                printf("\nServer disconnected.\n");
                exit(0);
            }
            printf("%s", buffer);
            fflush(stdout);
        }
    } else {
        // Parent Process: Continuously reads user input from terminal and sends to server
        while (1) {
            memset(message, 0, sizeof(message));
            fgets(message, sizeof(message), stdin);
            send(sock, message, strlen(message), 0);
        }
    }

    close(sock);
    return 0;
}