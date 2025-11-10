CFLAGS=-Wall
CC=gcc
SOURCES=main.c lshell.c commands.c socket.c lua_interop.c
OBJS=main.o lshell.o commands.o socket.o lua_interop.o
LFLAGS=-llua5.4


all: lshell

debug: $(OBJS)
	$(CC) $(CFLAGS) -c tests.c -o main.o
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS)

clean:
	rm -f *.o *.exe lshell

lshell: $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJS) $(LFLAGS)

lshell.o: lshell.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

commands.o: commands.c
	$(CC) $(CFLAGS) -c $< -o $@

socket.o: socket.c
	$(CC) $(CFLAGS) -c $< -o $@

lua_interop.o: lua_interop.c
	$(CC) $(CFLAGS) -c $< -o $@
