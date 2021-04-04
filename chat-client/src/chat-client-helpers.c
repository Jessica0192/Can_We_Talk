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
			printf("socket creation failed...\n");
			exit(0);
	}
	// printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// printf("Connected to the server..\n");

	// cientIncomingThread
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
			printf("connection with the server failed...\n");
			exit(0);
	}
	return client_socket;
}

void * clientIncomingThread(ClientInfoDef *clientInfo)
{
	int client_socket = create_socket(clientInfo->servaddr);
	// printf("In clientIncomingThread = %d\n", client_socket);

	// Create a new message queue
	key_t msg_key = ftok("./test", 0);
	if(msg_key == -1)
	{
		printf ("Cannot get msg_key\n");
		exit(1);
	}
	int msg_id = -1;
	if((msg_id = msgget(msg_key, 0)) == -1)
	{
		msg_id = msgget(msg_key, IPC_CREAT | 0660);
		if(msg_id == -1)
		{
			fflush(stdout);
			printf ("Cannot get msg_id ... \n");
			exit(1);
		}
		sleep(1);
	}

	clientInfo->msg_id = msg_id;

	printf ("----- clientInfo->msg_id - (%d) \n", clientInfo->msg_id);
	sleep(1);

	struct myMsg msg;
	while (true)
	{
			// sendMsg(clientInfo->msg_id);
			// // int msg_size = sizeof(msg) - sizeof(long);
			// int msg_size = sizeof(msg);
			// msg.text = {"test"};
			msg.type = 1;

			char str[20];
			sprintf(str, " - %d", (rand() % (30 - 10 + 1)) + 10);
			strncpy(msg.text, "hello", 20);
			strcat(msg.text,str);
			int rtn_code = msgsnd(msg_id, (void *) &msg, sizeof(msg.text), IPC_NOWAIT);

			// To handle if not able to send a message
			if (rtn_code == -1)
			{
				printf ("xxSend msg failed - (%d-%d) - %d \n", msg_id,clientInfo->msg_id,  rtn_code);
			}
			// else
			// {
			// 	printf ("Send msg passed: (%d--%d) - %s \n", msg_id, clientInfo->msg_id, msg.text);
			// }
			sleep(2);
	}
	// close the socket
	close(client_socket);
}

void * clientOutGoingThread(ClientInfoDef *clientInfo)
{
	int client_socket = create_socket(clientInfo->servaddr);
	// printf("In clientOutGoingThread = %d\n", client_socket);

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

int sendMsg(int msgID)
{
	struct myMsg msg;
	int msg_size = sizeof(msg) - sizeof(long);

	// msg.status = (rand() % (30 - 10 + 1)) + 10;

	printf ("Send msg %s - %d \n", msg.text, msgID);

	int rtn_code = msgsnd(msgID, &msg, msg_size, 0);

	// To handle if not able to send a message
	if (rtn_code == -1)
	{
		return 1;
	}
	return 0;
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
