#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer [1024] = {0};
	char *hello_msg = "Hello From Server!. Message received";
	
	if ( (server_fd = socket(AF_INET,SOCK_STREAM, 0) ==0)
	{
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}
