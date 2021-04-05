/*
*
*	FILE: chat-client.c
*	PROJECT: A3
*	PROGRAMMER: Suka Sun
*	FIRST VERSION: 2021-03-20
*	DESCRIPTION: TBD
*
*/

#include "../inc/chat-client.h"
void * clientIncomingThread(ClientInfoDef *clientInfo);
void * clientOutGoingThread(ClientInfoDef *clientInfo);
void * open_ui(ClientInfoDef *clientInfo);

// #include "../inc/chat-client-helpers.h"
// #include "../inc/chat-client-ncurses.h"

int main()
{
		// pthread_t  pInc, pOut;
		ClientInfoDef clientInfo;
		clientInfo.type = 312;
    clientInfo.servaddr.sin_family = AF_INET;
    clientInfo.servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientInfo.servaddr.sin_port = htons(PORT);
		clientInfo.msg_size = sizeof(clientInfo.srvMsgRec) - sizeof(long);
		clientInfo.msg_id = -1;

		// Create a new message queue
		key_t msg_key = ftok("./test2", 0);
		if(msg_key == -1)
		{
			printf ("Cannot get msg_key\n");
			exit(1);
		}
		if((clientInfo.msg_id = msgget(msg_key, 0)) == -1)
		{
			clientInfo.msg_id = msgget(msg_key, IPC_CREAT | 0660);
			if(clientInfo.msg_id == -1)
			{
				fflush(stdout);
				printf ("Cannot get clientInfo.msg_id ... \n");
				exit(1);
			}
			sleep(1);
		}
		printf("[CLIENT-1] %p \n", (uintptr_t)&clientInfo);
		// Create and launch clientIncomingThread
		if (pthread_create(&(clientInfo.pInc), NULL, clientIncomingThread, &clientInfo))
		{
			printf ("[CLIENT] : clientIncomingThread FAILED\n");
			fflush(stdout);
		}

    printf("[CLIENT-2] %p \n", (uintptr_t)&clientInfo);
		// Create and launch clientOutGoingThread
		if (pthread_create(&(clientInfo.pOut), NULL, clientOutGoingThread, &clientInfo))
		{
			printf ("[CLIENT] : clientOutGoingThread FAILED\n");
			fflush(stdout);
		}

		printf("[CLIENT-3] %p \n", (uintptr_t)&clientInfo);
		// Open UI
		open_ui(&clientInfo);

		printf("[CLIENT-4] %p \n", (uintptr_t)&clientInfo);

		// Waiting for a thread
		printf("Waiting for threads to exit... \n");
		int* ptr;
		pthread_join(clientInfo.pInc, &ptr);
		pthread_join(clientInfo.pOut, &ptr);

    printf("DONE \n");
}
