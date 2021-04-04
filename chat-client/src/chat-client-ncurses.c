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
  int index = 0;
  struct myMsg srvMsgRec;
  int msg_size = sizeof(srvMsgRec) - sizeof(long);
  int tmp = 111;


  key_t msg_key = ftok("./test", 0);
	if(msg_key == -1)
	{
		printf ("Cannot get msg_key\n");
		exit(1);
	}
	int msg_id = -1;
	if((msg_id = msgget(msg_key, 0)) == -1)
	{
		msg_id = msgget(msg_key, IPC_CREAT | 0660);
		if(msg_id == -1)
		{
			fflush(stdout);
			printf ("Cannot get msg_id ... \n");
			exit(1);
		}
		sleep(1);
	}


  mvprintw(index % 10, 1, "Enter Password: (%d)         ", clientInfo->msg_id);
  refresh();
  while (true)
  {
    if( msgrcv(msg_id, &srvMsgRec, msg_size, 1, 0) == -1 )
    {
       tmp = 222;
    }
    else
    {
        tmp = 333;
    }

    mvprintw(index % 10, 1, "Enter Password: (%d--%d) %d - %d : %s         ", msg_id, clientInfo->msg_id, index, tmp, srvMsgRec.text );
    index = index + 1;
    refresh();
    // sleep(2);
  }
}

// void * monitorInputWindow(void *client_input_window)
void * monitorInputWindow(ClientInfoDef* clientInfo)
{
  int index = 0;
  char *input = (char *)malloc(10);
  while (true)
  {
    wgetnstr(clientInfo->client_input_window, input, sizeof(input));
    wclear(clientInfo->client_input_window);
    index = index + 1;
    refresh();
    sleep(2);
  }
}


void open_ui(ClientInfoDef clientInfo)
{
  initscr();			/* Start curses mode 		  */

  int client_input_window_size = 1;
  int parent_x, parent_y, new_x, new_y;
  getmaxyx(stdscr, parent_y, parent_x);
  clientInfo.client_msg_window = newwin(parent_y - client_input_window_size, parent_x, 0, 0);
  clientInfo.client_input_window = newwin(client_input_window_size, parent_x, parent_y - client_input_window_size, 2);

  // client_msg_window
  pthread_t  pMsg, pInput;
  if (pthread_create(&pMsg, NULL, monitorMsgWindow, &clientInfo))
  {
    printf ("[CLIENT] : pMsg\n");
    fflush(stdout);
  }

  // client_input_window
  if (pthread_create(&pInput, NULL, monitorInputWindow, &clientInfo))
  {
    printf ("[CLIENT] : pInput\n");
    fflush(stdout);
  }

  int* ptr;
  pthread_join(pMsg, &ptr);
  pthread_join(pInput, &ptr);
  printf("DONE \n");


	refresh();			/* Print it on to the real screen */
	getch();			/* Wait for user input */
	endwin();			/* End curses mode		  */

}
