C      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic
LIBS_PATH = -L./fannF/src

.SUFFIXES: .c .o

.PHONY: all clean

all: logic botsonly training fann runfann

logic: logic.o -lncurses -lmenu

botsonly: botsonly.o -lncurses

training: training.o -lncurses -lm

fann: fann.o
	$(C) $(CFLAGS) fann.o $(LIBS_PATH) -lfann

runfann: runfann.o
	$(C) $(CFLAGS) runfann.o $(LIBS_PATH) -lfann -lncurses
	
clean:
	rm -f $(wildcard *.o)
	rm -f logic
	rm -f botsonly
	rm -f fann
	rm -f runfann
