C      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic
LIBS_PATH = -L./fannF/src -lfann -lncurses -lm -lmenu -lrt -pthread

.SUFFIXES: .c .o

.PHONY: all clean run rundemo

all: snake demo

snake: main.o logic.o server.o menus.o config.h
	${CC} ${CFLAGS} -o snake $^ ${LIBS_PATH}

demo: demo.o demologic.o demomenu.o democonfig.h
	${CC} ${CFLAGS} -o demo $^ ${LIBS_PATH}
	
run: snake  
	LD_LIBRARY_PATH=./fannF/src/ ./snake

rundemo: demo
	LD_LIBRARY_PATH=./fannF/src/ ./demo

clean:
	rm -f $(wildcard *.o)
	rm -f snake
	rm -f demo
