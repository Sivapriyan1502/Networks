#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *hello_msg = "Hello from TCP Client!";
    char buffer[1024] = {0};

    // Step 1: Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    // Fixed assignment (using '=' instead of function parentheses)
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Step 2: Convert local loopback IP address
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Step 3: Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Step 4: Send message and read server reply
    send(sock, hello_msg, strlen(hello_msg), 0);
    printf("Hello message sent to server.\n");

    read(sock, buffer, 1024);
    printf("Server says: %s\n", buffer);

    // Step 5: Close socket
    close(sock);
    return 0;
}