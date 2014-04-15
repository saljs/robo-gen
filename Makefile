CC=gcc
CFLAGS=-w --std=gnu99 
LDFLAGS=-lwiringPi
SOURCES=main.c genetics.c robot.c sound.c
EXECUTABLE=robogen

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)

install:
	cp $(EXECUTABLE) ~/robogen-exec
