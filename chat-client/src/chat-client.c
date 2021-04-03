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
#include "../inc/chat-client-helpers.h"

int main()
{
		pthread_t  pInc, pOut;

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

		// Create and launch clientIncomingThread
		if (pthread_create(&pInc, NULL, clientIncomingThread, (void *)&servaddr))
		{
			printf ("[CLIENT] : clientIncomingThread FAILED\n");
			fflush(stdout);
		}

		// Create and launch clientOutGoingThread
		if (pthread_create(&pOut, NULL, clientOutGoingThread, (void *)&servaddr))
		{
			printf ("[CLIENT] : clientOutGoingThread FAILED\n");
			fflush(stdout);
		}

		// Waiting for a thread
		printf("Waiting for threads to exit... \n");
		int* ptr;
		pthread_join(pInc, &ptr);
		pthread_join(pOut, &ptr);

    printf("DONE \n");

}
