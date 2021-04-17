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


void validateSocketConn(ClientInfoDef *clientInfo, int socketReturnValue, char* msgPrefix)
{
  if (socketReturnValue <= 0) { // connection closed!
    fprintf(clientInfo->log, "[-->> %s] - [ERROR] Unable to send reponse or connection closed.\n, msgPrefix");
    fflush(clientInfo->log);
    system("clear");
    system("reset");
    printf("[ERROR] Server cannot be connected ...\n");
    exit(1);
  } else {
    fprintf(clientInfo->log, "[-->> %s] - Response sent (%d bytes).\n", msgPrefix, socketReturnValue);
  }
  fflush(clientInfo->log);
}


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
  int socketResp;

  struct myMsg msg;
  for (;;) {
    // sleep(1);
    validateSocketConn(
      clientInfo,
      write(client_socket, tmpmsg, sizeof(tmpmsg)),
      "IncomingThread-write"
    );
    bzero(buff, sizeof(buff));
    //to read the buffer
    validateSocketConn(
      clientInfo,
      read(client_socket, buff, sizeof(buff)),
      "IncomingThread-read"
    );
    //to log the message
    fprintf(clientInfo->log, "[-->> IncomingThread] - message from server: %s\n", buff);
    fflush(clientInfo->log);
    //to copy the buffer into the file
    strncpy(msg.text, buff, sizeof(buff));
    msg.type = 1;
    //to send the message to the server
    fprintf(clientInfo->log, "%u:[IncomingThread] Send message out ...\n", (unsigned)time(NULL));
    fflush(clientInfo->log);
    int rtn_code = msgsnd(clientInfo->msgIdUIRec, (void *) &msg, sizeof(msg.text), IPC_NOWAIT);
    //to handle if not able to send the message
    if (rtn_code == -1)
    {
      //to inform the user
      fprintf(clientInfo->log, "%u:[IncomingThread] Failed to send message out ...\n", (unsigned)time(NULL));
      fflush(clientInfo->log);
      exit(1);
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
  int socketResp;

  for (;;) {
    //to recive the message from the message queue
    int rtn = msgrcv(clientInfo->msgIdUISnd, &msgUISend, msgUISendSize, 1, 0);
    //write into the stream
    int textsize = 0;
    while (msgUISend.text[textsize] != '\0') textsize++;
    int slashindex = 0;
    while (msgUISend.text[slashindex] != '/') slashindex++;
    char *textIndex = &msgUISend.text;
    if ( textsize > 40 + slashindex)
    {
      char tmpBuff[80];
      strncpy(tmpBuff, msgUISend.text, 40 + slashindex);
      tmpBuff[40 + slashindex] = 0;
      validateSocketConn(
        clientInfo,
        write(client_socket, tmpBuff, 40 + slashindex),
        "OutgoingThread-write-first-40"
      );
      strncpy(tmpBuff, msgUISend.text, slashindex);
      tmpBuff[slashindex]=0;
      strcat(tmpBuff, "/");
      strcat(tmpBuff, (char *)(textIndex + 40 + slashindex ));
      fprintf(clientInfo->log, "[-->> OutgoingThread-write] second 40 chars sent: %s\n", tmpBuff);
      validateSocketConn(
        clientInfo,
        write(client_socket, tmpBuff, textsize - 40),
        "OutgoingThread-write-second-40"
      );
      fflush(clientInfo->log);
    }
    else
    {

      int tmpIndex = 0;
      bool letsOverwrite = false;
      while (tmpIndex < 40 + slashindex)
      {
        if(msgUISend.text[tmpIndex] == 0 )
        {
          msgUISend.text[tmpIndex] = ' ';
          letsOverwrite = true;
        }
        else if (letsOverwrite)
        {
          msgUISend.text[tmpIndex] = ' ';
        }
        tmpIndex++;
      }
      msgUISend.text[tmpIndex] = 0;

      validateSocketConn(
        clientInfo,
        write(client_socket, msgUISend.text, sizeof(msgUISend.text)),
        "OutgoingThread-write"
      );
    }
    //to log the message
    fprintf(clientInfo->log, "[-->> OutgoingThread] - message send to server: %s\n", msgUISend.text);
    fflush(clientInfo->log);
    validateSocketConn(
      clientInfo,
      read(client_socket, buff, sizeof(buff)),
      "OutgoingThread-read"
    );
  }

	// close the socket
	close(client_socket);
}
