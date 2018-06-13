C      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic
LIBS_PATH = -L./fannF/src -lfann -lncurses -lm -lmenu -lrt -pthread

.SUFFIXES: .c .o

.PHONY: all clean

all: snake botsonly

snake: main.o logic.o server.o menus.o config.h
	${CC} ${CFLAGS} -o snake $^ ${LIBS_PATH}

botsonly: botsonly.o
	
clean:
	rm -f $(wildcard *.o)
	rm -f snake
	rm -f botsonly
