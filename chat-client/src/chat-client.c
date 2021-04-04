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

// #include "../inc/chat-client-helpers.h"
// #include "../inc/chat-client-ncurses.h"

int main()
{
		// pthread_t  pInc, pOut;
		static ClientInfoDef clientInfo;

    clientInfo.servaddr.sin_family = AF_INET;
    clientInfo.servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientInfo.servaddr.sin_port = htons(PORT);


		// Create and launch clientIncomingThread
		if (pthread_create(&(clientInfo.pInc), NULL, clientIncomingThread, &clientInfo))
		{
			printf ("[CLIENT] : clientIncomingThread FAILED\n");
			fflush(stdout);
		}

		// Create and launch clientOutGoingThread
		if (pthread_create(&(clientInfo.pOut), NULL, clientOutGoingThread, &clientInfo))
		{
			printf ("[CLIENT] : clientOutGoingThread FAILED\n");
			fflush(stdout);
		}

		// Open UI
		open_ui(clientInfo);

		// Waiting for a thread
		printf("Waiting for threads to exit... \n");
		int* ptr;
		pthread_join(clientInfo.pInc, &ptr);
		pthread_join(clientInfo.pOut, &ptr);

    printf("DONE \n");
}
