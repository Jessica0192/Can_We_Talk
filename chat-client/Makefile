#
# FILE: Makefile
# PROJECT: A3
# PROGRAMMER: Suka Sun
# FIRST VERSION: 2021-03-20
# DESCRIPTION: TBD
#
# =======================================================
#                  DR Example
# =======================================================
# FINAL BINARY Target
./bin/chat-client :	./obj/chat-client.o ./obj/chat-client-helpers.o ./obj/chat-client-ncurses.o
	cc ./obj/chat-client.o ./obj/chat-client-helpers.o ./obj/chat-client-ncurses.o -o ./bin/chat-client -lpthread -lncurses
# =======================================================
#                     Dependencies
# =======================================================
./obj/chat-client.o : ./src/chat-client.c ./inc/chat-client.h
	cc -c ./src/chat-client.c -o ./obj/chat-client.o -lpthread

./obj/chat-client-helpers.o : ./src/chat-client-helpers.c ./inc/chat-client-helpers.h
	cc -c ./src/chat-client-helpers.c -o ./obj/chat-client-helpers.o -lpthread

./obj/chat-client-ncurses.o : ./src/chat-client-ncurses.c ./inc/chat-client-ncurses.h
	cc -c ./src/chat-client-ncurses.c -o ./obj/chat-client-ncurses.o -lpthread -lncurses

#
# =======================================================
# Other targets
# =======================================================
all : ./bin/chat-client
clean:
	rm -rf ./bin/*
	rm -f ./obj/*.o
