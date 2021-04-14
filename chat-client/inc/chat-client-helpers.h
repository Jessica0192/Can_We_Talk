/*
*
*	FILE: chat-client.h
*	PROJECT: A3
*	PROGRAMMER: Suka Sun
*	FIRST VERSION: 2021-03-20
*	DESCRIPTION: TBD
*
*/

#include "../inc/common.h"

#include <stdio.h>
#include <netdb.h>

int create_socket(struct sockaddr_in servaddr);
void * clientIncomingThread(ClientInfoDef *clientInfo);
void * clientOutGoingThread(ClientInfoDef *clientInfo);
