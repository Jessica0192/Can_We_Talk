/*
*  FILE: chat-client-ncurses.c
*  PROJECT: Can_We_Talk system - A4
*  PROGRAMMER: Suka Sun, Hoda Akrami
*  FIRST VERSION: 2021-04-07
*  DESCRIPTION: This is a file for the UI part of the chat system in the client side.
*
*/

#include "../inc/chat-client-ncurses.h"



/*  -- Method Header Comment

  Name  :   pushMessage
  Purpose :   to send the message to the server
  Inputs  :   ClientInfoDef*  clientInfo
        char*     msgtxt
  Outputs  :   log an error messageif required
  Returns  :   Nothing
*/
void pushMessage(ClientInfoDef* clientInfo, char* msgtxt)
{
  //to log the message
  fprintf(clientInfo->log, "[monitorInputWindow] msg: %s\n", msgtxt);
  struct myMsg msg;
  msg.type = 1;

  //to construct the message
  strncpy(msg.text, clientInfo->username, sizeof(clientInfo->username));
  strcat(msg.text, "/");
  strcat(msg.text, msgtxt);

  //to send the message to the message queue
  int rtn_code = msgsnd(clientInfo->msgIdUISnd, (void *) &msg, sizeof(msg.text), 0);

  //to check if sending the msg to the meaagae queue was successful
  if (rtn_code == -1)
  {
    fprintf(clientInfo->log, "[monitorInputWindow] Send msg failed - (msgIdUISnd:%d) - %d \n", clientInfo->msgIdUISnd,  rtn_code);
  }
  fflush(clientInfo->log);
}



/*  -- Method Header Comment

  Name  :   monitorMsgWindow
  Purpose :   to display the messages that has been sent by server
  Inputs  :   ClientInfoDef*  clientInfo
  Outputs  :   print the message in the window
  Returns  :   void *
*/
void * monitorMsgWindow(ClientInfoDef* clientInfo)
{
  int index = 0;
  struct myMsg xx;

  //to get the size of the message
  int msg_size = sizeof(xx) - sizeof(long);
  int j = 1;
  int newIndex = 2;

  fprintf(clientInfo->log, "[monitorMsgWindow] Started ...\n");
  fflush(clientInfo->log);

  //to start the receive message thread
  while (true)
  {
    //to receive the message from the server
    // fprintf(clientInfo->log, "%u:[monitorMsgWindow] - %d - Checking Message Queue\n", (unsigned)time(NULL), j);
    // fflush(clientInfo->log);
    int rtn = msgrcv(clientInfo->msgIdUIRec, &xx, msg_size, 0, IPC_NOWAIT);
    // fprintf(clientInfo->log, "%u:[monitorMsgWindow] - %d - Message Queue response received\n", (unsigned)time(NULL), j);
    // fflush(clientInfo->log);

    //to check if receiving the msg from the server was successful
    if( rtn  != -1 )
    {
      fprintf(clientInfo->log, "[monitorMsgWindow] Received MQ message\n");
      fflush(clientInfo->log);
      //to handle display only 10 lines in the window
      //when the number of messages are less than 10 lines
      if (j <= 10)
      {
        //to update the curser
        wmove(clientInfo->client_msg_window, index + 2, 1);
        //to print in the window
        wprintw(clientInfo->client_msg_window, "%s", xx.text);
        //to increament the index and print the message on the next line
        index = index + 1;
        //to refresh the window
        wrefresh(clientInfo->client_msg_window);
      }

      //to handle display only 10 lines in the window
      //when the number of messages are more than 10 lines
      if (j > 10)
      {
        //to update the curser
        wmove(clientInfo->client_msg_window, newIndex, 1);
        //to delete the oldest message
        wclrtoeol(clientInfo->client_msg_window);
        //to print in the window
        wprintw(clientInfo->client_msg_window, "[xxx.xxx.xxx.xxx]\t%s", xx.text);
        //to increament the index and print the message on the next line
        newIndex = newIndex + 1;
        //to refresh the window
        wrefresh(clientInfo->client_msg_window);

        //to refresh the variables when rewrite the lines
        if (j > 19)
        {
          j = 1;
          index = 0;
          newIndex = 2;
          }
      }

      j = j + 1;
    }
    sleep(0.5);
  }
}



/*  -- Method Header Comment

  Name    :   monitorInputWindow
  Purpose :   to get the user input and send it to the server
  Inputs  :   ClientInfoDef*  clientInfo
  Outputs :   the input message to the sender
  Returns :   void *
*/
void * monitorInputWindow(ClientInfoDef* clientInfo)
{
  int index = 0;
  char word[81] = {0};
  int ch;
  struct myMsg msg;
  msg.type = 1;
  int maxrow, maxcol, row = 2, col = 2;

  fprintf(clientInfo->log, "[monitorInputWindow] Started ...\n");
  fflush(clientInfo->log);

  //to get the maximum numbet of rows and culmns (window size)
  getmaxyx(clientInfo->client_input_window, maxrow, maxcol);

  //to position curser at top
  wmove(clientInfo->client_input_window, 2, 2);

  //to send the message to the server that client started the connection
  pushMessage(clientInfo, "Hello I am here.");
  int i;

  //to start the send thread
  while (true)
  {
    //get the inputs from the keyboard
    for (i = 0; (ch = wgetch(clientInfo->client_input_window)) != '\n'; i++)
    {
      word[i] = ch;

      //to handle when user enter >>bye<<
      if (word[0] == '>' && word[1] == '>' && word[2] == 'b' && word[3] == 'y' && word[4] == 'e' && word[5] == '<' && word[6] == '<')
      {
         //to send the message to the server
         pushMessage(clientInfo, ">>bye<<");
         //to close the window
         endwin();
      }

      //to check if within window
      if (col++ < maxcol-2)
      {
        // wprintw(clientInfo->client_input_window, "%c", word[i]);
      }
      else                                /* last char pos reached */
      {
        col = 4;
        if (row == maxrow-2)              /* last line in the window */
        {
          scroll(clientInfo->client_input_window);                    /* go up one line */
          row = maxrow-2;                                             /* stay at the last line */
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

      //to handle 80 characters limit
      if( i >= 79 )
      {
        i++;
        //to stop getting characters from the user
        while((ch = mvwgetch(clientInfo->client_input_window,row, col)) != '\n')
        {
          fprintf(clientInfo->log, "[monitorInputWindow] Hit 80 characters limit ...");
          fflush(clientInfo->log);
          word[i] = ch;
        }
        break;
      }
    }
    word[i] = 0;
    pushMessage(clientInfo, word);

    for (int k = 2; k < maxcol-2; k++)
    {
      //move the curser to every line
      wmove(clientInfo->client_input_window, k, 2);
      //to clean the line that curser point to
      wclrtoeol(clientInfo->client_input_window);
      wrefresh(clientInfo->client_input_window);
    }
    //to move the curser to the first line
    wmove(clientInfo->client_input_window, 2, 2);
    box(clientInfo->client_input_window, 0, 0);

  }
}



/*  -- Method Header Comment

  Name  :   open_ui
  Purpose :   to create the message and input windows
  Inputs  :   ClientInfoDef*  clientInfo
  Outputs  :   2 windows
  Returns  :   void *
*/
void * open_ui(ClientInfoDef * clientInfo)
{
  //to start curses mode
  initscr();

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

  //to create the displaye message window
  clientInfo->client_msg_window = newwin(msg_height, msg_width, msg_starty, msg_startx);
  scrollok(clientInfo->client_msg_window, TRUE);

  //to create the input window
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

  //to refresh the windows
  wrefresh(clientInfo->client_msg_window);
  wrefresh(clientInfo->client_input_window);
  getch();      /* Wait for user input */
  endwin();      /* End curses mode */

}
