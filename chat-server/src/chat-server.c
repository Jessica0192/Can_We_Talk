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
void *socketThread(void *clientSocket);
void sigIntHandler(int signal);
typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	int IPaddress[max_num_clients];
	char* userID[max_num_clients];

	
} serverThread;

// global variable to keep count of the number of clients ...
static int numClients;
static int cur_IP;
static serverThread* serv_th;
static int global_serv_socket;
static int counter;
volatile pthread_t	     tid[10];

int main(void)
{printf("\nHELLO1\n");
	serv_th = (serverThread*)malloc(sizeof(serverThread));
	 
	int                server_socket, client_socket;
	int                client_len;
	struct sockaddr_in client_addr, server_addr;
	int                len, i;
	//pthread_t	     tid[3];
	int                whichClient;=
	signal (SIGINT, sigIntHandler);
	numClients = 0;
	counter=0;
	 /*
	   * obtain a socket for the server
	   */
	if ((server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
	{
	  printf ("[SERVER] : socket() FAILED\n");
	  return 1;
	}
	printf("[SERVER] : socket() successful\n");
	global_serv_socket = server_socket;=
	  /*
	   * initialize our server address info for binding purposes
	   */ 
	memset (&server_addr, 0, sizeof (server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	server_addr.sin_port = htons (PORT);
	if (bind (server_socket, (struct sockaddr *)&server_addr, sizeof (server_addr)) < 0) 
	{
	  printf ("[SERVER] : bind() FAILED\n");
	  close (server_socket);
	  return 2;
	}
	printf ("[SERVER] : bind() successful\n");
	  
	  /*
	   * start listening on the socket
	   */ 
	if (listen (server_socket, 5) < 0) 
	{
	  printf ("[SERVER] : listen() - FAILED.\n");
	  close (server_socket);
	  return 3;
	}
	printf ("[SERVER] : listen() successful\n");
	numClients = 1;


	 /*
	   * for this server, run an endless loop that will
	   * accept incoming requests from a remote client.
	   * the server will create a thread to handle the
	   * request, and the parent will continue to listen for the
	   * next request - up to 3 clients
	   */
	 while (numClients <= max_num_clients) //numClients != 0 && numClients <= max_num_clients 
	  {

		if (numClients == 0 || numClients > max_num_clients)
		{
			break;
		}

		printf("[SERVER] : Ready to accept()\n");
		fflush(stdout);
	
		counter++;
	
	       /*
		* accept a packet from the client
		*/
		client_len = sizeof (client_addr);

		if ((client_socket = accept (server_socket,(struct sockaddr *)&client_addr, &client_len)) < 0) 
		{
		  printf ("[SERVER] : accept() FAILED\n");
		  fflush(stdout);	
		  return 4;
		} 
		//struct sockaddr_in sa;
		char str[INET_ADDRSTRLEN];

		// now get it back and print it
		inet_ntop(AF_INET, &(client_addr.sin_addr), str, INET_ADDRSTRLEN);

		if(numClients == 1 && counter == 1)
		{ 
			numClients--;
		}

		printf("%s\n", str); // prints "192.0.2.33"
		serv_th->IPaddress[numClients]= client_socket; //new
		cur_IP = serv_th->IPaddress[numClients]; //new added thing
		
		numClients++;

		
		printf("[SERVER] : received a packet from CLIENT-%02d\n", numClients);
		fflush(stdout);	

		/*
		 * rather than fork and spawn the execution of the command within a 
		 * child task - let's take a look at something else we could do ...
		 *
		 * ... we'll create a thread of execution within this task to take
		 * care of executing the task.  We'll be looking at THREADING in a 
		 * couple modules from now ...
		 */
		//not here
		if (pthread_create(&(tid[(numClients-1)]), NULL, socketThread, (void *)&client_socket))
		{
		  printf ("[SERVER] : pthread_create() FAILED\n");
		  fflush(stdout);	
		  return 5;
		}
		printf("[SERVER] : pthread_create() successful for CLIENT-%02d\n", numClients);
		fflush(stdout);
		
	  } 
	//exit the thread
	//pthread_exit((void*)1);
	
	printf("\n[SERVER] : All clients have returned - exiting ...\n");
	close (server_socket);

	return 0; 
}



// 
// Socket handler - this function is called (spawned as a thread of execution)
//

void *socketThread(void *clientSocket)
  // used for accepting incoming command and also holding the command's response
  char buffer[BUFSIZ];
  char message[BUFSIZ];
  int sizeOfRead;
  int timeToExit;
  char* parsed_userID;
  char* parsed_msg = NULL;
  // remap the clientSocket value (which is a void*) back into an INT
  int clSocket = *((int*)clientSocket);

  /* Clear out the input Buffer */
  memset(buffer,0,BUFSIZ);
  // increment the numClients
  int iAmClient = numClients;	// assumes that another connection from another client 
				// hasn't been created in the meantime

  read (clSocket, buffer, BUFSIZ);
  parsed_userID = strtok(buffer, "/");  
  printf("parsedUSERID: %s\n", parsed_userID);
  serv_th->userID[numClients - 1] = parsed_userID;

  //parsed_msg = strtok(NULL, "/");
  parsed_msg = parsed_userID;
  printf("hello function for socket thread\n");
if (parsed_userID != 0){
  while(strcmp(parsed_msg,">>bye<<") != 0)
  { 
	 time_t t = time(NULL);
         struct tm tm = *localtime(&t);
    /* we're actually not going to execute the command - but we could if we wanted */

	char* null_str = NULL;

	if (parsed_msg)
	{ 

		printf("numCleints: %d\n", numClients);
		sprintf (message, "[%d] [%s] << %s (%02d:%02d:%02d)", clSocket, parsed_userID, parsed_msg, tm.tm_hour, tm.tm_min, tm.tm_sec);

	    for (int i = 0; i < numClients; i++)
	    {


		    if (serv_th->IPaddress[i] != clSocket)
		    {
		
			printf("WRITING MESSAGES!!!!!! [%d]\n", clSocket);
			   printf("[%d] [%s] << %s (%02d:%02d:%02d)\n", clSocket, parsed_userID, parsed_msg, tm.tm_hour, tm.tm_min, tm.tm_sec);


			if(serv_th->IPaddress[i] != 0 && write (serv_th->IPaddress[i], message, strlen(message)) == -1)
			{
				printf("Socket write failed\n");
			}
		    }

	   }

	}


 
    // clear out and get the next command and process
    memset(buffer,0,BUFSIZ);
    memset(message,0,sizeof(message));

    read (clSocket, buffer, BUFSIZ);
  }

}

		for (int i = 0; i < numClients - 1; i++)
		{
			serv_th->userID[i] = (char *) malloc(100);
			serv_th->userID[i+1] = (char *) malloc(100);
			serv_th->IPaddress[i] = serv_th->IPaddress[i + 1];
			strcpy(serv_th->userID[i], serv_th->userID[i + 1]);
		}
		
		serv_th->IPaddress[numClients-1] = 0;
		strcpy(serv_th->userID[numClients-1], "");

		//iAmClient--;
  // decrement the number of clients
  numClients--;

  printf("CLOSE@ socket in socket thread\n");
  close(clSocket);

  printf("AFTER closing, numClients: %d\n", numClients);
  
  timeToExit = iAmClient;


  printf ("[SERVER (Thread-%02d)] : closing socket\n", iAmClient);

 // pthread_exit((void*)1);
int rc = 0;
 printf("Cancel the thread\n");

//if (numClients == 0) {exit(0);}//

for (int i = 0; i < 3; i++){pthread_exit(tid[i]);printf("\nterminate %d\n", i);}
//pthread_exit(0);
//pthread_cancel(numClients);
//pthread_exit(tid[iAmClient-1]);
 // rc = pthread_cancel((tid[(numClients-1)]));
  printf("\nRC AFTER CLOSIN SOCKET - %d\n", rc);



  clSocket--;
	
  return 0;
}



/*
* FUNCTION: sigIntHandler
* PURPOSE: The function takes signal number which is raised. This function is called when SIGINT signal is raised
* and it deattach from shared-memory
* PARAMETER: int signal - signal number which is raised
* RETURNS: void - there's no return value
*/
void sigIntHandler(int signal)
{
      	// disable CTRL-C support! but only 3 times max!
	printf ("SIGINT IN CHAT-SERVER\n");

      	close (global_serv_socket);
	exit(1);
}

