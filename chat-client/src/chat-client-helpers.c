/*
*
*	FILE: chat-client-helpers.c
*	PROJECT: Can_We_Talk system - A4
*	PROGRAMMER: Suka Sun, Hoda Akrami
*	FIRST VERSION: 2021-04-07
*	DESCRIPTION: This is a file for chat system for makeing a connection beween client and server from the client side
*
*/

#include "../inc/chat-client-helpers.h"



/*  -- Method Header Comment

	Name	:   clientIncomingThread
	Purpose :   to receive the emssage from the server
	Inputs	:   ClientInfoDef*	clientInfo
	Outputs	:   log the messages
	Returns	:   void *
*/
void * clientIncomingThread(ClientInfoDef *clientInfo)
{

  int client_socket;
  //to log the message
  fprintf(clientInfo->log, "[-->> IncomingThread] Started ...\n");
  //clear the buffer
  fflush(clientInfo->log);

  //get a socket for communications
  if ((client_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
    //to inform the user
    printf ("[CLIENT] : Getting Client Socket - FAILED\n");
    return NULL;
  }

  // attempt a connection to server
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
    // sleep(1);
    write(client_socket, tmpmsg, sizeof(tmpmsg));
    bzero(buff, sizeof(buff));
    //to read the buffer
    read(client_socket, buff, sizeof(buff));
    //to log the message
    fprintf(clientInfo->log, "[-->> IncomingThread] - message from server: %s\n", buff);
    fflush(clientInfo->log);
    //to copy the buffer into the file
    strncpy(msg.text, buff, sizeof(buff));
    msg.type = 1;
    //to send the message to the server
    int rtn_code = msgsnd(clientInfo->msgIdUIRec, (void *) &msg, sizeof(msg.text), 0);
    //to handle if not able to send the message
    if (rtn_code == -1)
    {
      //to inform the user
      printf ("xxSend msg failed - (%d) - %d \n", clientInfo->msgIdUIRec,  rtn_code);
      // exit(1);
    }
  }

  //to close the socket
  close(client_socket);
}



/*  -- Method Header Comment

	Name	:   clientOutGoingThread
	Purpose :   to send the emssage to the server
	Inputs	:   ClientInfoDef*	clientInfo
	Outputs	:   log the messages
	Returns	:   void *
*/
void * clientOutGoingThread(ClientInfoDef *clientInfo)
{

  int client_socket;
  //to log the message
  fprintf(clientInfo->log, "[--<< OutgoingThread] Started ...\n");
  fflush(clientInfo->log);

  //get a socket for communications
  fflush(stdout);
  //to handle if not able to connect to server
  if ((client_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
    //to log the message
    fprintf(clientInfo->log, "[-->> OutgoingThread] - [CLIENT] : Connect to Server - FAILED\n");
    fflush(clientInfo->log);
    exit(1);
  }
  clientInfo->outgoingConnEstablished = true;

  //attempt a connection to server
  fflush(stdout);
  //to handle if not able to connect to server
  if (connect (client_socket, (struct sockaddr *)&clientInfo->servaddr, sizeof(clientInfo->servaddr)) < 0)
  {
     //to inform the user
     printf ("[CLIENT] : Connect to Server - FAILED\n");
     //to close the connection
     close (client_socket);
     return NULL;
  }

  char buff[MAX];
  int n;
  struct myMsg msgUISend;
  //to get the message size
  int msgUISendSize = sizeof(msgUISend) - sizeof(long);

  for (;;) {
    //to recive the message from the message queue
    int rtn = msgrcv(clientInfo->msgIdUISnd, &msgUISend, msgUISendSize, 1, 0);
    //write into the stream
    write(client_socket, msgUISend.text, sizeof(msgUISend.text));
    //to log the message
    fprintf(clientInfo->log, "[-->> OutgoingThread] - message send to server: %s\n", msgUISend.text);
    fflush(clientInfo->log);
    read(client_socket, buff, sizeof(buff));
    // sleep(10);
  }

	// close the socket
	close(client_socket);
}
