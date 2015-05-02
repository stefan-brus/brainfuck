CFLAGS = -Wall -g
CC = cc

all: brainfuck

brainfuck: main.o
	$(CC) $(CFLAGS) main.o -o brainfuck

clean:
	rm -f brainfuck *.o

