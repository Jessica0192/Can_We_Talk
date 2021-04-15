#
# FILE: Makefile
# PROJECT: A3
# PROGRAMMER: MARIA MALININA
# FIRST VERSION: 2021-03-20
# DESCRIPTION: This file is a Makefile to compile all the source codes. It also has ability to
# clean bin and obj directory
# this makefile will compile and and all source
# found in the "DR" directory.  This represents a sample
# development directory structure and project
#
# =======================================================
#                  DR Example
# =======================================================
# FINAL BINARY Target
./bin/chat-server :	./obj/chat-server.o
	cc ./obj/chat-server.o -o ./bin/chat-server -lpthread
# =======================================================
#                     Dependencies
# =======================================================
./obj/chat-server.o : ./src/chat-server.c
	cc -c ./src/chat-server.c -o ./obj/chat-server.o -lpthread

#
# =======================================================
# Other targets
# =======================================================
all : ./bin/chat-server
clean:
	rm -rf ./bin/chat*
	rm -f ./obj/*.o
