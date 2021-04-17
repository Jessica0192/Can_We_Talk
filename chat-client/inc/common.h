/*
*	FILE: common.h
*	PROJECT: A3
*	PROGRAMMER: Suka Sun, Hoda Akrami
*	FIRST VERSION: 2021-03-20
*	DESCRIPTION: This file includes the definitions shared between
*              the chat-client.c and chat-client-ncurses.c
*/

#include <ncurses.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>

#include <errno.h>

#define MAX 80
#define PORT 5000
#define SA struct sockaddr

struct myMsg
{
	long type;
	char text[200];
};

typedef struct
{
	// the following is a requriement of UNIX/Linux
	long type;
  pthread_t  pInc;
  pthread_t  pOut;
  struct sockaddr_in servaddr;
  struct myMsg srvMsgUIRec;
  struct myMsg srvMsgSnd;
  int msg_size ;
  WINDOW *client_msg_window;
  WINDOW *client_input_window;
	int msgIdUIRec;
  int msgIdUISnd;
  FILE *log;
  char username[20];
  char server[20];
  bool incomingConnEstablished;
  bool outgoingConnEstablished;

} ClientInfoDef;

// Send message
int sendMsg(int id);
