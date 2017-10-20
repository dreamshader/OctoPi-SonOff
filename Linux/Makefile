
CCFLAGS=-g -Wall 
CCDEFINE=-DRASPBERRYPI
CC=gcc

CCINCLUDE=

PROGRAM = shutdown_server
SOURCES = shutdown_server.c
HEADERS =
LIBS    = -lrt
all: ${PROGRAM}

${PROGRAM}: ${SOURCES} ${HEADERS}
	${CC} ${CCFLAGS} ${CCDEFINE} ${CCINCLUDE} ${SOURCES} ${LIBS} -o ${PROGRAM}

clean:
	rm -rf $(PROGRAM) *.o

refresh:
	touch *.c *.h 


.PHONY: all
