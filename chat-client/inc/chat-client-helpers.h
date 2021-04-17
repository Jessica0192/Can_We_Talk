/*
*	FILE: chat-client.h
*	PROJECT: A3
*	PROGRAMMER: Suka Sun, Hoda Akrami
*	FIRST VERSION: 2021-03-20
*	DESCRIPTION: This file includes the function definition for the chat-client.c
*/

#include "../inc/common.h"

#include <stdio.h>
#include <netdb.h>

void * clientIncomingThread(ClientInfoDef *clientInfo);
void * clientOutGoingThread(ClientInfoDef *clientInfo);
void validateSocketConn(ClientInfoDef *clientInfo, int socketReturnValue, char* msgPrefix);
