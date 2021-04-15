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

int main(int argc,char* argv[])
{

		ClientInfoDef clientInfo;
		clientInfo.type = 312;
    clientInfo.servaddr.sin_family = AF_INET;
    clientInfo.servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientInfo.servaddr.sin_port = htons(PORT);
		clientInfo.msg_size = sizeof(clientInfo.srvMsgUIRec) - sizeof(long);
		clientInfo.msgIdUIRec = -1;
    clientInfo.msgIdUISnd = -1;
    clientInfo.log = fopen("x2.log", "a+");

    // Parse input args
    int counter;
    bool userIsGiven = false;
    bool serverIsGiven = false;
    char *argTmp;
    for(counter=0;counter<argc;counter++)
    {
      // fprintf(clientInfo.log, "[Main] argv[%d]: %s \n",counter,argv[counter]);
      if(strncmp(argv[counter], "--user=", strlen("--user")) == 0)
      {
        argTmp = strtok(argv[counter], "=");
        argTmp = strtok(NULL, "=");
        fprintf(clientInfo.log, "[Main] User is given as argv[%d]: %s \n",counter,argv[counter]);
        strncpy(clientInfo.username, argTmp, strlen(argTmp)+1);
        fprintf(clientInfo.log, "[Main] The given user name is : %s (length=%d/%d)\n", clientInfo.username, strlen(clientInfo.username), strlen(argTmp));
        userIsGiven = true;
      }
      else if(strncmp(argv[counter], "--server=", strlen("--server")) == 0)
      {
        fprintf(clientInfo.log, "[Main] Server is given as argv[%d]: %s \n",counter,argv[counter]);
        serverIsGiven = true;
      }

    }
    fflush(clientInfo.log);

		// Create a new message queue - msg UI receive
    system("touch mq.rec.key");
		key_t msg_key = ftok("./mq.rec.key", 0);
		if(msg_key == -1)
		{
			//printf ("Cannot get msg_key\n");
			exit(1);
		}
		if((clientInfo.msgIdUIRec = msgget(msg_key, 0)) == -1)
		{
			clientInfo.msgIdUIRec = msgget(msg_key, IPC_CREAT | 0660);
			if(clientInfo.msgIdUIRec == -1)
			{
				fflush(stdout);
				//printf ("Cannot get clientInfo.msgIdUIRec ... \n");
				exit(1);
			}
			sleep(1);
		}

    // Create a new message queue - msg UI send
    system("touch mq.snd.key");
		key_t msg_key_snd = ftok("./mq.snd.key", 0);
		if(msg_key_snd == -1)
		{
			//printf ("Cannot get msg_key_snd\n");
			exit(1);
		}
		if((clientInfo.msgIdUISnd = msgget(msg_key_snd, 0)) == -1)
		{
			clientInfo.msgIdUISnd = msgget(msg_key_snd, IPC_CREAT | 0660);
			if(clientInfo.msgIdUISnd == -1)
			{
				fflush(stdout);
				//printf ("Cannot get clientInfo.msgIdUISnd ... \n");
				exit(1);
			}
			sleep(1);
		}

    fprintf(clientInfo.log, "[Main] msgIdUIRec=%d, msgIdUISnd=%d\n", clientInfo.msgIdUIRec, clientInfo.msgIdUISnd);
    fflush(clientInfo.log);

		//printf("[CLIENT-1] %p \n", (uintptr_t)&clientInfo);
		// Create and launch clientIncomingThread
		if (pthread_create(&(clientInfo.pInc), NULL, clientIncomingThread, &clientInfo))
		{
			//printf ("[CLIENT] : clientIncomingThread FAILED\n");
			fflush(stdout);
		}

		// Create and launch clientOutGoingThread
		if (pthread_create(&(clientInfo.pOut), NULL, clientOutGoingThread, &clientInfo))
		{
			//printf ("[CLIENT] : clientOutGoingThread FAILED\n");
			fflush(stdout);
		}

		// Open UI
		open_ui(&clientInfo);


		// Waiting for threads
		int* ptr;
		pthread_join(clientInfo.pInc, &ptr);
		pthread_join(clientInfo.pOut, &ptr);

   // printf("DONE \n");
}
