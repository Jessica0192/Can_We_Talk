/*
*
*	FILE: chat-client.c
*	PROJECT: A3
*	PROGRAMMER: Suka Sun
*	FIRST VERSION: 2021-03-20
*	DESCRIPTION: TBD
*
*/

#include "../inc/chat-client-helpers.h"

int create_socket(void *servaddr)
{
	int client_socket;
	// socket create and varification
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			printf("socket creation failed...\n");
			exit(0);
	}
	printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	printf("Connected to the server..\n");
	// cientIncomingThread
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
			printf("connection with the server failed...\n");
			exit(0);
	}
	return client_socket;
}

void * clientIncomingThread(void *servaddr)
{
	int client_socket = create_socket(servaddr);
	printf("In clientIncomingThread = %d\n", client_socket);

	// close the socket
	close(client_socket);
}

void * clientOutGoingThread(void *servaddr)
{
	int client_socket = create_socket(servaddr);
	printf("In clientOutGoingThread = %d\n", client_socket);

	// close the socket
	close(client_socket);
}

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


// bool send_msg(char user[], char message[])
// {
// 	return true;
// }
//
// char *receive_msg(int count) {
//   char msg[MAX] = {"Hello World"};
// 	return buff
// }
