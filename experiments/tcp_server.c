#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello_msg = "Hello From Server! Message received";
    int opt = 1;
    
    // Step 1: Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Exam Tip: Forcefully attach socket to the port 8080 to prevent "Address already in use"
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Step 2: Configure address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Step 3: Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Step 4: Listen
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on PORT %d...\n", PORT);

    // Step 5: Accept client connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    // Step 6: Read from client and write back reply
    read(new_socket, buffer, 1024);
    printf("Client says: %s\n", buffer);

    write(new_socket, hello_msg, strlen(hello_msg));
    printf("Hello message sent to client\n");

    // Step 7: Close sockets
    close(new_socket);
    close(server_fd);
    return 0;
}