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

  int                client_socket, len, done;
  int                whichClient = 123;
  struct sockaddr_in server_addr;
  struct hostent*    host = gethostbyname("localhost");

  fprintf(clientInfo->log, "[-->> IncomingThread] Started ...\n");
  fflush(clientInfo->log);

  /*
   * initialize struct to get a socket to host
   */
  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  memcpy (&server_addr.sin_addr, host->h_addr, host->h_length);
  server_addr.sin_port = htons (5000);

  /*
  * get a socket for communications
  */
  fflush(stdout);
  if ((client_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf ("[CLIENT-%d] : Getting Client Socket - FAILED\n", whichClient);
    return NULL;
  }

  /*
  * attempt a connection to server
  */
  fflush(stdout);
  if (connect (client_socket, (struct sockaddr *)&server_addr,sizeof (server_addr)) < 0)
  {
    printf ("[CLIENT-%d] : Connect to Server - FAILED\n", whichClient);
    close (client_socket);
    return NULL;
  }

   char buff[MAX];
   char * tmpmsg = {"000/dummy"};
   int n;

   struct myMsg msg;
   for (;;) {
      sleep(1);
      write(client_socket, tmpmsg, sizeof(tmpmsg));
      // printf("[Incoming] Input sent, socket: %d\n", client_socket);
      bzero(buff, sizeof(buff));
      // printf("[Incoming] Sent, read response now ...\n");
      read(client_socket, buff, sizeof(buff));

      strncpy(msg.text, buff, sizeof(buff));
      msg.type = 1;
      // strcat(msg.text, str);
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

  sleep(2);

  int                client_socket, len, done;
  int                whichClient = 123;
  struct sockaddr_in server_addr;
  struct hostent*    host = gethostbyname("localhost");

  fprintf(clientInfo->log, "[--<< OutgoingThread] Started ...\n");
  fflush(clientInfo->log);

  /*
   * initialize struct to get a socket to host
   */
  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  memcpy (&server_addr.sin_addr, host->h_addr, host->h_length);
  server_addr.sin_port = htons (5000);

  /*
  * get a socket for communications
  */
  fflush(stdout);
  if ((client_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
     printf ("[CLIENT-%d] : Getting Client Socket - FAILED\n", whichClient);
     return NULL;
  }

  /*
  * attempt a connection to server
  */
  fflush(stdout);
  if (connect (client_socket, (struct sockaddr *)&server_addr,sizeof (server_addr)) < 0)
  {
     printf ("[CLIENT-%d] : Connect to Server - FAILED\n", whichClient);
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
    read(client_socket, buff, sizeof(buff));
    sleep(10);
  }

	// close the socket
	close(client_socket);
}
