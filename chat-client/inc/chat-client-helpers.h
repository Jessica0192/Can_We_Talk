/*
*
*	FILE: chat-client.h
*	PROJECT: A3
*	PROGRAMMER: Suka Sun
*	FIRST VERSION: 2021-03-20
*	DESCRIPTION: TBD
*
*/

// #include <netdb.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <pthread.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <stdbool.h>
// #include <ncurses.h>

#include "../inc/common.h"
// 
// #define MAX 80
// #define PORT 8080
// #define SA struct sockaddr

int create_socket(struct sockaddr_in servaddr);
void * clientIncomingThread(ClientInfoDef *clientInfo);
void * clientOutGoingThread(ClientInfoDef *clientInfo);
void func(int client_socket);
