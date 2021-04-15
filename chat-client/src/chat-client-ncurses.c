/*
*
*	FILE: chat-client-ncurses.c
*	PROJECT: A3
*	PROGRAMMER: Suka Sun, Hoda Akrami
*	FIRST VERSION: 2021-03-20
*	DESCRIPTION: TBD
*
*/

#include "../inc/chat-client-ncurses.h"

// void * monitorMsgWindow(void *client_msg_window)
void * monitorMsgWindow(ClientInfoDef* clientInfo)
{

  //printf("[Receiver] %p \n", (uintptr_t)clientInfo);
  //printf("[Receiver] clientInfo->type = %d \n", clientInfo->type);
  //printf("[Receiver] clientInfo->msgIdUIRec = %d \n", clientInfo->msgIdUIRec);

  int index = 0;
  struct myMsg xx;
  int msg_size = sizeof(xx) - sizeof(long);

  while (true)
  {
    int rtn = msgrcv(clientInfo->msgIdUIRec, &xx, msg_size, 1, 0);
    if( rtn  != -1 )
    {
      //to update the curser
      wmove(clientInfo->client_msg_window, index + 2, 1);
      // mvprintw(index % 10, 1, "[%s]--Message Received: %d -(msgIdUIRec: %d) %d : %s \n",
      wprintw(clientInfo->client_msg_window, "Message Received: %d -(msgIdUIRec: %d) %d : %s",
        clientInfo->type,
        clientInfo->msgIdUIRec,
        index, xx.text
      );

      index = index + 1;
      wrefresh(clientInfo->client_msg_window);
    }
    else
    {
      mvprintw(index % 10, 1, "Message Missed: (%s) %d - (%d) %d",
        clientInfo->servaddr.sin_addr.s_addr,
        clientInfo->type, clientInfo->msgIdUIRec, index
      );
      index = index + 1;
      refresh();
    }
    // refresh();
    sleep(1);
    // wclear(clientInfo->client_msg_window);
  }
}

// void * monitorInputWindow(void *client_input_window)
void * monitorInputWindow(ClientInfoDef* clientInfo)
{
  int index = 0;
  char word[81] = {0};
  int ch;

  struct myMsg msg;
  msg.type = 1;

  int maxrow, maxcol, row = 2, col = 2;

  getmaxyx(clientInfo->client_input_window, maxrow, maxcol);          /* get window size */

  //to position curser at top
  wmove(clientInfo->client_input_window, 2, 2);

  while (true)
  {
    //wgetnstr(clientInfo->client_input_window, input, 80);
    //wclear(clientInfo->client_input_window);
    //index = index + 1;
    //wrefresh(clientInfo->client_input_window);
    // sleep(2);
     for (int i = 0; (ch = wgetch(clientInfo->client_input_window)) != '\n'; i++)
     {
       word[i] = ch;                       /* '\n' not copied */
       if (col++ < maxcol-2)               /* if within window */
       {
         wprintw(clientInfo->client_input_window, "%c", word[i]);      /* display the char recv'd */
       }
       else                                /* last char pos reached */
       {
         col = 4;
         if (row == maxrow-2)              /* last line in the window */
         {
           scroll(clientInfo->client_input_window);                    /* go up one line */
           row = maxrow-2;                 /* stay at the last line */
           wmove(clientInfo->client_input_window, row, col);           /* move cursor to the beginning */
           wclrtoeol(clientInfo->client_input_window);                 /* clear from cursor to eol */
           box(clientInfo->client_input_window, 0, 0);                 /* draw the box again */
         }
//write else if compare str == bye to exit
         else
         {
           row++;
           wmove(clientInfo->client_input_window, row, col);           /* move cursor to the beginning */
           wrefresh(clientInfo->client_input_window);
           wprintw(clientInfo->client_input_window, "%c", word[i]);    /* display the char recv'd */
         }
       }
     }

     fprintf(clientInfo->log, "[-- monitorInputWindow] msg: %s\n", word);
     strncpy(msg.text, word, sizeof(word));
     int rtn_code = msgsnd(clientInfo->msgIdUISnd, (void *) &msg, sizeof(msg.text), 0);
     if (rtn_code == -1)
     {
       fprintf(clientInfo->log, "[-- monitorInputWindow] Send msg failed - (msgIdUISnd:%d) - %d \n", clientInfo->msgIdUISnd,  rtn_code);
     }
     fflush(clientInfo->log);

     wmove(clientInfo->client_input_window, 2, 2);           /* move cursor to the beginning */
     wclrtoeol(clientInfo->client_input_window);                 /* clear from cursor to end of line(eol) */
  }
}


void * open_ui(ClientInfoDef * clientInfo)
{

  //printf("[open_ui] %p \n", (uintptr_t)clientInfo);
  initscr();			/* Start curses mode 		  */

  int client_input_window_size = 5;
  int parent_x, parent_y, new_x, new_y;

  int chat_height = 5;
  int chat_width  = COLS - 2;
  int chat_startx = 1;
  int chat_starty = LINES - chat_height;

  int msg_height = LINES - chat_height - 1;
  int msg_width  = COLS;
  int msg_startx = 0;
  int msg_starty = 0;

  //to get maximum size of y and x of the screen
  getmaxyx(stdscr, parent_y, parent_x);

  clientInfo->client_msg_window = newwin(msg_height, msg_width, msg_starty, msg_startx);
  scrollok(clientInfo->client_msg_window, TRUE);

  clientInfo->client_input_window = newwin(chat_height, chat_width, chat_starty, chat_startx);
  scrollok(clientInfo->client_input_window, TRUE);

  //move the cursor to the specified location
  wmove(clientInfo->client_msg_window, 2, 2);

  //to have a border around the window
  box(clientInfo->client_msg_window, 0, 0);
  box(clientInfo->client_input_window, 0, 0);

  //to print message on the bar
  mvwprintw(clientInfo->client_msg_window, 1, parent_x/2 - 5, "Messages");
  mvwprintw(clientInfo->client_input_window, 1, parent_x/2 - 10, "Outgoing Messages");

  //to have a bar on top of the window
  mvwchgat(clientInfo->client_msg_window, 1, 1, parent_x, A_STANDOUT, COLOR_WHITE, NULL);
  mvwchgat(clientInfo->client_input_window, 1, 1, parent_x, A_STANDOUT, COLOR_WHITE, NULL);

  // client_msg_window
  pthread_t  pMsg, pInput;
  if (pthread_create(&pMsg, NULL, monitorMsgWindow, clientInfo))
  {
    //printf ("[CLIENT] : pMsg\n");
    fflush(stdout);
  }

  // client_input_window
  if (pthread_create(&pInput, NULL, monitorInputWindow, clientInfo))
  {
    //printf ("[CLIENT] : pInput\n");
    fflush(stdout);
  }

  int* ptr;
  pthread_join(pMsg, &ptr);
  pthread_join(pInput, &ptr);
  //printf("DONE \n");

	//refresh();			/* Print it on to the real screen */
	wrefresh(clientInfo->client_msg_window);
	wrefresh(clientInfo->client_input_window);
	getch();			/* Wait for user input */
	endwin();			/* End curses mode		  */

}
