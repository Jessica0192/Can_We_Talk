/*
*
*	FILE: chat-client-ncurses.c
*	PROJECT: A3
*	PROGRAMMER: Suka Sun
*	FIRST VERSION: 2021-03-20
*	DESCRIPTION: TBD
*
*/

#include "../inc/chat-client-ncurses.h"

// void * monitorMsgWindow(void *client_msg_window)
void * monitorMsgWindow(ClientInfoDef* clientInfo)
{

  printf("[Receiver] %p \n", (uintptr_t)clientInfo);
  printf("[Receiver] clientInfo->type = %d \n", clientInfo->type);
  printf("[Receiver] clientInfo->msg_id = %d \n", clientInfo->msg_id);

  int index = 0;
  struct myMsg xx;
  int msg_size = sizeof(xx) - sizeof(long);

  while (true)
  {
    int rtn = msgrcv(clientInfo->msg_id, &xx, msg_size, 1, 0);
    if( rtn  != -1 )
    {
      mvprintw(index % 10, 1, "[%s]--Message Received: %d -(msg_id: %d) %d : %s \n",
        clientInfo->client_msg_window,
        clientInfo->type,
        clientInfo->msg_id,
        index, xx.text
      );
      index = index + 1;
    }
    else
    {
      mvprintw(index % 10, 1, "Message Missed: (%s) %d - (%d) %d",
        clientInfo->servaddr.sin_addr.s_addr,
        clientInfo->type, clientInfo->msg_id, index
      );
      index = index + 1;

    }
    refresh();
    sleep(1);
    // wclear(clientInfo->client_msg_window);
  }
}

// void * monitorInputWindow(void *client_input_window)
void * monitorInputWindow(ClientInfoDef* clientInfo)
{
  // int index = 0;
  // char *input = (char *)malloc(200);
  // while (true)
  // {
  //   // wgetnstr(clientInfo->client_input_window, input, sizeof(input));
  //   // wclear(clientInfo->client_input_window);
  //   index = index + 1;
  //   refresh();
  //   sleep(2);
  // }
}


void * open_ui(ClientInfoDef * clientInfo)
{


  printf("[open_ui] %p \n", (uintptr_t)clientInfo);
  initscr();			/* Start curses mode 		  */

  int client_input_window_size = 1;
  int parent_x, parent_y, new_x, new_y;
  getmaxyx(stdscr, parent_y, parent_x);


  clientInfo->client_msg_window = newwin(parent_y - client_input_window_size, parent_x, 0, 0);
  // clientInfo->client_input_window = newwin(client_input_window_size, parent_x, parent_y - client_input_window_size, 2);

  WINDOW *client_input_window = newwin(client_input_window_size, parent_x, parent_y - client_input_window_size, 2);


  // sleep(2);

  // wclear(clientInfo->client_msg_window);

  // client_msg_window
  pthread_t  pMsg, pInput;
  if (pthread_create(&pMsg, NULL, monitorMsgWindow, clientInfo))
  {
    printf ("[CLIENT] : pMsg\n");
    fflush(stdout);
  }

  // // client_input_window
  // if (pthread_create(&pInput, NULL, monitorInputWindow, clientInfo))
  // {
  //   printf ("[CLIENT] : pInput\n");
  //   fflush(stdout);
  // }


  int index = 0;
  char *input = (char *)malloc(200);
  while (true)
  {
    wgetnstr(client_input_window, input, sizeof(input));
    wclear(client_input_window);
    index = index + 1;
    refresh();
    sleep(2);
  }

  int* ptr;
  pthread_join(pMsg, &ptr);
  // pthread_join(pInput, &ptr);
  // printf("DONE \n");


	refresh();			/* Print it on to the real screen */
	getch();			/* Wait for user input */
	endwin();			/* End curses mode		  */

}
