C      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic
LIBS_PATH = -L./fannF/src

.SUFFIXES: .c .o

.PHONY: all clean

all: logic botsonly

logic: logic.o
	$(C) $(CFLAGS) logic.o -o logic $(LIBS_PATH) -lfann -lncurses -lm -lmenu

botsonly: botsonly.o -lncurses
	
clean:
	rm -f $(wildcard *.o)
	rm -f logic
	rm -f botsonly
