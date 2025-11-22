CFLAGS=-Wall
CC=gcc
SOURCES=main.c lshell.c commands.c socket.c lua_interop.c win32_socket.c
OBJS=main.o lshell.o commands.o socket.o lua_interop.o win32_socket.o

#Change this to match your OS - only these two options are available.
#OS=OS_LINUX 
OS=OS_WINDOWS

# Define the lua location manually on windows. Linux, use your package manager
LUA_DIR=./lua-5.4.8/install/lib
LFLAGS=-L$(LUA_DIR) -llua # might have to change depending on your system

all: lshell

#debug: $(OBJS) #debug build is WIP / disabled for now
#	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o lshell-debug

clean:
	rm -f *.o *.exe lshell a.out

lshell: $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJS) $(LFLAGS) -o lshell --define-macro $(OS)
		

lshell.o: lshell.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

commands.o: commands.c
	$(CC) $(CFLAGS) -c $< -o $@

linux_socket.o: linux_socket.c
	$(CC) $(CFLAGS) -c $< -o $@

win32_socket.o: win32_socket.c
	$(CC) $(CFLAGS) -c $< -o $@

lua_interop.o: lua_interop.c
	$(CC) $(CFLAGS) -c $< -o $@
