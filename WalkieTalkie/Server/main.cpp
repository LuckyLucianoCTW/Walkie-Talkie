#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <WinSock2.h>
#include <string.h> 
#include <winsock.h>
#include <socketapi.h>
#include <ws2tcpip.h>
#include <stdio.h>
#define ATTACHEDPORT 8080  


int main()
{
    sockaddr_in address;
    int addrlen = sizeof(address);
    SOCKET server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST,
        (char*)&opt, 4))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(ATTACHEDPORT);   
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    SOCKET new_socket;
    if ((new_socket = accept(server_fd, (struct sockaddr*) & address,
        (socklen_t*)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }  
    int valread;
    char buffer[1024] = { 0 };
    const char* hello = "Hello from server";
    recv(new_socket, buffer, 1024, 0);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    return 0;
}