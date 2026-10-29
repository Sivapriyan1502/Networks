#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define MAX_CLIENTS 10

int main() {
    int server_fd, new_socket, client_socket[MAX_CLIENTS], activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1025];
    fd_set readfds;
    char *welcome_msg = "Connected to TCP Chat Server!\n";

    // Initialize all client socket array elements to 0 (not checked/empty)
    for (i = 0; i < MAX_CLIENTS; i++) {
        client_socket[i] = 0;
    }

    // 1. Create master socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Multi-User Chat Server is listening on PORT %d...\n", PORT);

    while (1) {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add master socket to set
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add child sockets to set
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];
            
            // If valid socket descriptor, add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);
            
            // Highest file descriptor number is needed for select()
            if (sd > max_sd)
                max_sd = sd;
        }

        // Wait for activity on one of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Select error");
        }

        // If something happened on the master socket, it's an incoming connection
        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            printf("New connection: socket fd is %d, IP is: %s, Port: %d\n", 
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Send welcome message to the new client
            send(new_socket, welcome_msg, strlen(welcome_msg), 0);

            // Add new socket to array of clients
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
                }
            }
        }

        // Else, it's some IO operation on some other client socket
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                // Check if it was for closing, or read the incoming message
                valread = read(sd, buffer, 1024);
                if (valread == 0) {
                    // Client disconnected
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Client disconnected, IP: %s, Port: %d\n", 
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(sd);
                    client_socket[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    printf("Received message: %s", buffer);

                    // Broadcast message to all other connected clients
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int dest_sd = client_socket[j];
                        if (dest_sd != 0 && dest_sd != sd) {
                            send(dest_sd, buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}