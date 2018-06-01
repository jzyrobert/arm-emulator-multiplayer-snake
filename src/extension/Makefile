CC      = gcc
CFLAGS  = -Wall -O3 -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic -pg

.SUFFIXES: .c .o

.PHONY: all clean

all: logic

logic: logic.o -lncurses -lmenu

clean:
	rm -f $(wildcard *.o)
	rm -f logic
