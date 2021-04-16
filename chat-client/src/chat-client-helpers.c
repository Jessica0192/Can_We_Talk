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

void * clientIncomingThread(ClientInfoDef *clientInfo)
{

  int client_socket;

  fprintf(clientInfo->log, "[-->> IncomingThread] Started ...\n");
  fflush(clientInfo->log);

  /*
  * get a socket for communications
  */
  if ((client_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf ("[CLIENT] : Getting Client Socket - FAILED\n");
    return NULL;
  }

  /*
  * attempt a connection to server
  */
  fflush(stdout);
  if (connect (client_socket, (struct sockaddr *)&clientInfo->servaddr,sizeof(clientInfo->servaddr)) < 0)
  {
    fprintf(clientInfo->log, "[-->> IncomingThread] - [CLIENT] : Connect to Server - FAILED\n");
    fflush(clientInfo->log);
    close (client_socket);
    exit(1);
  }
  clientInfo->incomingConnEstablished = true;

  char buff[MAX];
  char * tmpmsg = {"000/dummy"};
  int n;

  struct myMsg msg;
  for (;;) {
    sleep(1);
    write(client_socket, tmpmsg, sizeof(tmpmsg));
    bzero(buff, sizeof(buff));
    read(client_socket, buff, sizeof(buff));
    fprintf(clientInfo->log, "[-->> IncomingThread] - message from server: %s\n", buff);
    fflush(clientInfo->log);
    strncpy(msg.text, buff, sizeof(buff));
    msg.type = 1;
    int rtn_code = msgsnd(clientInfo->msgIdUIRec, (void *) &msg, sizeof(msg.text), 0);
    if (rtn_code == -1)
    {
      printf ("xxSend msg failed - (%d) - %d \n", clientInfo->msgIdUIRec,  rtn_code);
      // exit(1);
    }
  }

  close(client_socket);
}

void * clientOutGoingThread(ClientInfoDef *clientInfo)
{

  int client_socket;
  fprintf(clientInfo->log, "[--<< OutgoingThread] Started ...\n");
  fflush(clientInfo->log);

  /*
  * get a socket for communications
  */
  fflush(stdout);
  if ((client_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
    fprintf(clientInfo->log, "[-->> OutgoingThread] - [CLIENT] : Connect to Server - FAILED\n");
    fflush(clientInfo->log);
    exit(1);
  }
  clientInfo->outgoingConnEstablished = true;

  /*
  * attempt a connection to server
  */
  fflush(stdout);
  if (connect (client_socket, (struct sockaddr *)&clientInfo->servaddr, sizeof(clientInfo->servaddr)) < 0)
  {
     printf ("[CLIENT] : Connect to Server - FAILED\n");
     close (client_socket);
     return NULL;
  }

  char buff[MAX];
  int n;
  struct myMsg msgUISend;
  int msgUISendSize = sizeof(msgUISend) - sizeof(long);

  for (;;) {
    int rtn = msgrcv(clientInfo->msgIdUISnd, &msgUISend, msgUISendSize, 1, 0);
    write(client_socket, msgUISend.text, sizeof(msgUISend.text));
    fprintf(clientInfo->log, "[-->> OutgoingThread] - message send to server: %s\n", msgUISend.text);
    fflush(clientInfo->log);
    read(client_socket, buff, sizeof(buff));
    // sleep(10);
  }

	// close the socket
	close(client_socket);
}
