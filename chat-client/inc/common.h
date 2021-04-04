

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
#define PORT 8080
#define SA struct sockaddr

typedef struct
{
	// the following is a requriement of UNIX/Linux
	long type;
  pthread_t  pInc;
  pthread_t  pOut;
  struct sockaddr_in servaddr;
  WINDOW *client_msg_window;
  WINDOW *client_input_window;
	int msg_id;

} ClientInfoDef;


struct myMsg
{
	long type;
	char text[20];
};

// Send message
int sendMsg(int id);
