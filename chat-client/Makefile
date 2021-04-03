#
# FILE: Makefile
# PROJECT: A3
# PROGRAMMER: Suka Sun
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
./bin/chat-client :	./obj/chat-client.o
	cc ./obj/chat-client.o -o ./bin/chat-client -lpthread
# =======================================================
#                     Dependencies
# =======================================================
./obj/chat-client.o : ./src/chat-client.c
	cc -c ./src/chat-client.c -o ./obj/chat-client.o -lpthread

#
# =======================================================
# Other targets
# =======================================================
all : ./bin/chat-client
clean:
	rm -rf ./bin/*
	rm -f ./obj/*.o
