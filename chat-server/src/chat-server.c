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


#include "../inc/chat-server.h"
/*
* FILE          : chat-server.c
* PROJECT       : SENG2030 - Assignment #4
* PROGRAMMER    : Maria Malinina, Jessica Sim
* FIRST VERSION : 2020-03-27
* DESCRIPTION   :
* This project contains the functionality of a chat server. We implement a TCP/IP
* protocol here to make the server and client communicate. The server supports
* multiple clients, each time one client sends a message to the chat, the server
* receives it, processes and sends it back to a client, formatted appropriately,
* and then the client can display the message in its UI. The idea of sockets is im-
* plemented here and whenever a client connects to server, a new socketThread is run-
* ning.
* If the message had more than 40 characters, the server will truncate the message
* at 40 characters.
* The server goes off when all of the clients said ">>bye<<" in the chat.
*/

//global static variable that'll hold the current client's ip
static char* client_ip;

int main(void)
{
  //allocate memory for the server thread
  serv_th = (serverThread*)malloc(sizeof(serverThread));


  //instantiate client's and server's sockets
  int                server_socket, client_socket;
  int                client_len;
  struct sockaddr_in client_addr, server_addr;
  int                len, i;


  int                whichClient;

  //handling signal
  signal (SIGINT, sigIntHandler);
  numClients = 0;
  counter=0;


  /*
  * obtain a socket for the server
  */
  if ((server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
    return 1;
  }

   //instantiate the global server socket
  global_serv_socket = server_socket;

  /*
  * initialize our server address info for binding purposes
  */
  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  server_addr.sin_port = htons (PORT);

  //start binding the socket
  if (bind (server_socket, (struct sockaddr *)&server_addr, sizeof (server_addr)) < 0)
  {
    //close the server socket
    close (server_socket);
    return 2;
  }

  /*
  * start listening on the socket
  */
  if (listen (server_socket, 5) < 0)
  {
    //close the server socket
    close (server_socket);
    return 3;
  }

  //reset the number of clients
  numClients = 1;


  /*
  * for this server, run an endless loop that will
  * accept incoming requests from a remote client.
  * the server will create a thread to handle the
  * request, and the parent will continue to listen for the
  * next request - up to 3 clients
  */
  while (numClients <= max_num_clients) //loop through and accept client's requests
  {

   //if there are no more clients or the number of clients exceeded
   // and if it happened - exit the server
    if (numClients == 0 || numClients > max_num_clients)
    {
      break; //break out the loop
      sleep(1);
    }

    //increase the counter
    counter++;

    /*
    * accept a packet from the client
    */
    client_len = sizeof (client_addr);


    //start accepting client's socket
    if ((client_socket = accept (server_socket,(struct sockaddr *)&client_addr, &client_len)) < 0)
    {
      return 4;
    }
    
    //string object that will contain the client's ip address
    char str[INET_ADDRSTRLEN];

    // deal with the current client's ip address
    inet_ntop(AF_INET, &(client_addr.sin_addr), str, INET_ADDRSTRLEN);
    client_ip = inet_ntoa(client_addr.sin_addr);


    //the number of clients getting to 1, we decrease the number of clients
    if(numClients == 1 && counter == 1)
    {
      numClients--;
    }

    serv_th->IPaddress[numClients]= client_socket; //set the ip address
    cur_IP = serv_th->IPaddress[numClients]; //save it to the local variable 

    numClients++;//increase the nunmber of clients

    /*
   create a thread of execution within this task to take
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


/*
* FUNCTION : socketThread()
* PURPOSE  : Socket handler - this function is called (spawned as a thread of execution)
* PARAMETER: ivoid* clientSocket - the client's socket
* RETURNS  : void - there's no return value
*/
void *socketThread(void *clientSocket){
  // used for accepting incoming command and also holding the command's response
  char buffer[BUFSIZ];
  char message[BUFSIZ]; //this is where we will store the formatted message from the client


  int sizeOfRead;
  int timeToExit;
  char* parsed_userID; //this is where we'll store the userid
  char* parsed_msg = NULL;

  // remap the clientSocket value (which is a void*) back into an INT
  int clSocket = *((int*)clientSocket);

  /* Clear out the input Buffer */
  memset(buffer,0,BUFSIZ);

  // increment the numClients
  int iAmClient = numClients;  // assumes that another connection from another client
  // hasn't been created in the meantime

  //read client's socket
  read (clSocket, buffer, BUFSIZ);
  parsed_userID = strtok(buffer, "/");
  parsed_msg = strtok(NULL, "/");

  //save the current user id
  serv_th->userID[numClients - 1] = parsed_userID;

  //make sure user id is not null
  if (parsed_userID != 0)
  {
    //we need to loop until we receive a >>bye<< message from the client
    while(strncmp(parsed_msg,">>bye<<", 7) != 0)
    {
      
		//make sure user id is not null
	if(strcmp(parsed_userID,"000") != 0 && parsed_msg != NULL)
	{	//instantiate a new time instance
		time_t t = time(NULL);

		//get the current time
		struct tm tm = *localtime(&t);

		//make sure the current message is not null - otherwise if we didn't have this statement,
		if (parsed_msg)	//the server would be receiving messages from clients every minute
		{
		 	//start displaying messages for each of the clients
			  for (int i = 0; i < numClients; i++)
			  {
			    serv_th->userID[i] = (char *) malloc(100);//allocate memory for the current user id


				//check if we are now dealing with the client who sent the message
			    if (strcmp(serv_th->userID[i], parsed_userID)!=0)
			    {
				//appropriately save the message in the formatted way and send it to client
				      if (serv_th->IPaddress[i] != clSocket)
				      {
					sprintf (message, "%s\t[%s] << %s \t\t(%02d:%02d:%02d)", client_ip, parsed_userID, parsed_msg, tm.tm_hour, tm.tm_min, tm.tm_sec);
				      }else{
					sprintf (message, "%s\t[%s] >> %s (%02d:%02d:%02d)", client_ip, parsed_userID, parsed_msg, tm.tm_hour, tm.tm_min, tm.tm_sec);
				      }

			      //check if the socket writing operation failed
			      if(serv_th->IPaddress[i] != 0 && write(serv_th->IPaddress[i], message, strlen(message)) == -1)
			      {
				printf("Socket write failed\n");
			      }
		           }
			   else
			   {
			      //appropriately save the message in the formatted way and send it to client
			      if (serv_th->IPaddress[i] != clSocket)
			      {
				sprintf (message, "%s\t[%s] << %s (%02d:%02d:%02d)", client_ip, serv_th->IPaddress[i], parsed_msg, tm.tm_hour, tm.tm_min, tm.tm_sec);
			      }else{
				sprintf (message, "%s\t[%s] >> %s (%02d:%02d:%02d)", client_ip, serv_th->IPaddress[i], parsed_msg, tm.tm_hour, tm.tm_min, tm.tm_sec);			      
			      } 

			      //check if the socket writing operation failed
			      if(serv_th->IPaddress[i] != 0 && write(serv_th->IPaddress[i], message, strlen(message)) == -1)
			      {
				printf("Socket write failed\n");
			      }
			    }
                     }
                }
         }
      
      //allocate memory
      memset(buffer,0,BUFSIZ);
      memset(message,0,sizeof(message));
      read (clSocket, buffer, BUFSIZ);  //start reading the clients socket
      parsed_userID = strtok(buffer, "/"); //deal with current user's id
      parsed_msg = strtok(NULL, "/"); //handle the message we recieved from the client
    }

  }

  //allocate the memory
  for (int i = 0; i < numClients - 1; i++)
  {
    serv_th->userID[i] = (char *) malloc(100);
    serv_th->userID[i+1] = (char *) malloc(100);
    serv_th->IPaddress[i] = serv_th->IPaddress[i + 1];
    strcpy(serv_th->userID[i], serv_th->userID[i + 1]);
  }

  //reset the ip address and user id values
  serv_th->IPaddress[numClients-1] = 0;
  strcpy(serv_th->userID[numClients-1], "");

  //decrease the number of clients
  numClients--;
  numClients--;

  //close the socket
  close(clSocket);

  timeToExit = iAmClient;

  if (numClients == 0)
  {
        //if the number of clients reached 0 - terminate the server
	exit(0);
  }

  
  //decrease the client's socket
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
