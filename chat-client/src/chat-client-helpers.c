/*
*
*	FILE: chat-client-helperz.c
*	PROJECT: A3
*	PROGRAMMER: Suka Sun
*	FIRST VERSION: 2021-03-20
*	DESCRIPTION: TBD
*
*/

#include "../inc/chat-client-helpers.h"

int create_socket(struct sockaddr_in servaddr)
{
	int client_socket;
	// socket create and varification
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			//printf("socket creation failed...\n");
			exit(0);
	}
	printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	printf("Connected to the server..\n");

	// cientIncomingThread
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
			//printf("connection with the server failed...\n");
			exit(0);
	}
	return client_socket;
}

void * clientIncomingThread(ClientInfoDef *clientInfo)
{

	//printf("[Sender] %p \n", (uintptr_t)clientInfo);
	//printf("[Sender] clientInfo->type = %d \n", clientInfo->type);
	//printf("[Sender] clientInfo->msg_id = %d \n", clientInfo->msg_id);

	int client_socket = create_socket(clientInfo->servaddr);

	struct myMsg msg;
	while (true)
	{
			msg.type = 1;
			char str[20];
			sprintf(str, " - %d", (rand() % (30 - 10 + 1)) + 10);
			strncpy(msg.text, "hello", 20);
			strcat(msg.text,str);
			// int rtn_code = msgsnd(clientInfo->msg_id, (void *) &msg, sizeof(msg.text), IPC_NOWAIT);
			int rtn_code = msgsnd(clientInfo->msg_id, (void *) &msg, sizeof(msg.text), 0);
			if (rtn_code == -1)
			{
				//printf ("xxSend msg failed - (%d) - %d \n", clientInfo->msg_id,  rtn_code);
				exit(1);
			}
			sleep(2);
	}
	// close the socket
	close(client_socket);
}

void * clientOutGoingThread(ClientInfoDef *clientInfo)
{
	// int client_socket = create_socket(clientInfo->servaddr);
	printf("xxxxIn clientOutGoingThread = %d\n", client_socket);
  printf("%d\n", clientInfo->servaddr);
  // int MAX = 512;
  char buff[MAX];
  int n;

  bzero(buff, sizeof(buff));
  printf("Enter the string : ");
  n = 0;
  while ((buff[n++] = getchar()) != '\n')
    ;
  printf("Input received\n");

  write(client_socket, buff, sizeof(buff));
  printf("Input sent\n");
  bzero(buff, sizeof(buff));
  printf("Sent, read response now ...");
  read(client_socket, buff, sizeof(buff));
  printf("From Server : %s", buff);
  sleep(10);
	// close the socket
	close(client_socket);
}

// void func(int client_socket)
// {
//     char buff[MAX];
//     int n;
//     for (;;) {
//         bzero(buff, sizeof(buff));
// 				sleep(10);
//         printf("Enter the string : ");
//         n = 0;
//         while ((buff[n++] = getchar()) != '\n')
//             ;
//         write(client_socket, buff, sizeof(buff));
//         bzero(buff, sizeof(buff));
//         read(client_socket, buff, sizeof(buff));
//         printf("From Server : %s", buff);
//         if ((strncmp(buff, "exit", 4)) == 0) {
//             printf("Client Exit...\n");
//             break;
//         }
//     }
// }
