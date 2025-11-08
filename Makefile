CFLAGS=-Wall
CC=gcc
SOURCES=main.c lshell.c commands.c socket.c lua_interop.c
OBJS=main.o lshell.o commands.o socket.o lua_interop.o

all: lshell

clean:
	rm -f *.o *.exe lshell

lshell: $(OBJS)
	$(CC) $(CFLAGS) -l $(OBJS)

lshell.o: lshell.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

commands.o: commands.c
	$(CC) $(CFLAGS) -c $< -o $@

socket.o: socket.c
	$(CC) $(CFLAGS) -c $< -o $@
