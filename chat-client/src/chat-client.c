#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void func(int client_socket)
{
    char buff[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
				sleep(10);
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(client_socket, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(client_socket, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main()
{
    int client_socket, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }

		printf("connected to the server..\n");

    // function for chat
    func(client_socket);

    // close the socket
    close(client_socket);
}
