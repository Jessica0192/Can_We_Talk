/*
*	FILE: chat-client-ncurses.h
*	PROJECT: A3
*	PROGRAMMER: Suka Sun, Hoda Akrami
*	FIRST VERSION: 2021-03-20
*	DESCRIPTION: This file includes the function definition for the chat-client-ncurses.c
*/


#include "../inc/common.h"

void * open_ui(ClientInfoDef *clientInfo);
void * monitorMsgWindow(ClientInfoDef* clientInfo);
void * monitorInputWindow(ClientInfoDef* clientInfo);
