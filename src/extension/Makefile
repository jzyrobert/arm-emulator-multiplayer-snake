CC      = gcc
CFLAGS  = -LfannF -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: logic botsonly training fann runfann

logic: logic.o -lncurses -lmenu

botsonly: botsonly.o -lncurses

training: training.o -lncurses

fann: fann.o -lfann

runfann: runfann.o -lncurses -lfann

clean:
	rm -f $(wildcard *.o)
	rm -f logic
	rm -f botsonly
	rm -f fann
	rm -f runfann
