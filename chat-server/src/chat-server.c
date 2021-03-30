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
#define PORT 8080
#define max_num_clients 10
void *socketThread(void *clientSocket);
typedef struct  
{
	// the following is a requriement of UNIX/Linux
	long type;

	char IPaddress[max_num_clients];
	char userID[max_num_clients];

	
} serverThread;

// global variable to keep count of the number of clients ...
static int numClients = 0;
static char* cur_IP;
static serverThread* serv_th;

int main(void)
{
	   serv_th = malloc(sizeof(serverThread));
	 
	  int                server_socket, client_socket;
	  int                client_len;
	  struct sockaddr_in client_addr, server_addr;
	  int                len, i;
	  pthread_t	     tid[3];
	  int                whichClient;

	 /*
   * obtain a socket for the server
   */

	  if ((server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
	  {
	    printf ("[SERVER] : socket() FAILED\n");
	    return 1;
	  }
	  printf("[SERVER] : socket() successful\n");
	
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


	 /*
	   * for this server, run an endless loop that will
	   * accept incoming requests from a remote client.
	   * the server will create a thread to handle the
	   * request, and the parent will continue to listen for the
	   * next request - up to 3 clients
	   */
	  while (numClients <= max_num_clients) 
	  {
		printf("[SERVER] : Ready to accept()\n");
		fflush(stdout);	
	
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

		printf("%s\n", str); // prints "192.0.2.33"
		strcpy(serv_th->IPaddress[numClients], str); //new
		strcpy(cur_IP, serv_th->IPaddress[numClients]); //new added thing


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
		if (pthread_create(&(tid[(numClients-1)]), NULL, socketThread, (void *)&client_socket))
		{
		  printf ("[SERVER] : pthread_create() FAILED\n");
		  fflush(stdout);	
		  return 5;
		}

		printf("[SERVER] : pthread_create() successful for CLIENT-%02d\n", numClients);
		fflush(stdout);	


	  }

	return 0;
}



// 
// Socket handler - this function is called (spawned as a thread of execution)
//

void *socketThread(void *clientSocket)
{
  // used for accepting incoming command and also holding the command's response
  char buffer[BUFSIZ];
  char message[BUFSIZ];
  int sizeOfRead;
  int timeToExit;
  char* parsed_userID;
  char* parsed_msg;

  // remap the clientSocket value (which is a void*) back into an INT
  int clSocket = *((int*)clientSocket);

  /* Clear out the input Buffer */
  memset(buffer,0,BUFSIZ);

  // increment the numClients
  int iAmClient = numClients;	// assumes that another connection from another client 
				// hasn't been created in the meantime

  read (clSocket, buffer, BUFSIZ);

  parsed_userID = strtok(buffer, "/");
  strcpy(serv_th->userID[numClients - 1], parsed_userID);
  parsed_msg = strtok(NULL, "/");

 
  

  while(strcmp(parsed_msg,">>bye<<") != 0)
  {
	 time_t t = time(NULL);
         struct tm tm = *localtime(&t);
    /* we're actually not going to execute the command - but we could if we wanted */

    for (int i = 0; i < numClients; i++){
    if (serv_th->IPaddress[i] != cur_IP){
		
		
	    sprintf (message, "[%s] [%s] >> %s (%02d:%02d:%02d)", cur_IP, parsed_userID, parsed_msg, tm.tm_hour, tm.tm_min, tm.tm_sec);
	    write (serv_th->IPaddress[i], message, strlen(message));
    }
   } 

    // clear out and get the next command and process
    memset(buffer,0,BUFSIZ);
    read (clSocket, buffer, BUFSIZ);
  }

  
  close(clSocket);
  
  // decrement the number of clients
  numClients--;

  // the return status will be the client # of this thread
  timeToExit = iAmClient;
  pthread_exit((void *)(timeToExit));
  printf ("[SERVER (Thread-%02d)] : closing socket\n", iAmClient);
	
  return 0;
}






