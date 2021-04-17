
/*
* FILE          : chat-server.h
* PROJECT       : SENG2030 - Assignment #4
* PROGRAMMER    : Maria Malinina, Jessica Sim
* FIRST VERSION : 2020-03-27
* DESCRIPTION   :
* This is headder file that contains the features we need to use in chat-server.c.
* Here, we have PORT, the mximum clients number, function prototypes and structure
* we use to handle server thread.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#define PORT 5000
#define max_num_clients 10

typedef struct
{
  // the following is a requriement of UNIX/Linux
  long type;

  int IPaddress[max_num_clients];
  char* userID[max_num_clients];


} serverThread;



void *socketThread(void *clientSocket);
void sigIntHandler(int signal);
char *truncString(char *str, int pos);

// global variable to keep count of the number of clients ...
static int numClients;
static int cur_IP;
static serverThread* serv_th;
static int global_serv_socket;
static int counter;
volatile pthread_t       tid[10];

