CC=gcc
# SOURCES=main.c lshell.c commands.c socket.c lua_interop.c win32_socket.c
OBJS=main.o lshell.o commands.o linux_socket.o lua_interop.o win32_socket.o
SRC_DIR=src/
#Change this to match your OS - only these two options are available.
#OS=OS_LINUX 
OS=OS_WINDOWS
CFLAGS=-Wall --define-macro $(OS)

# Define the lua location manually on windows. Linux, use your package manager
LUA_DIR=./lua-5.4.8/install/lib
LFLAGS=-L$(LUA_DIR) -llua -lws2_32 # might have to change depending on your system

all: lshell

#debug: $(OBJS) #debug build is WIP / disabled for now
#	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o lshell-debug

clean:
	rm -f *.o *.exe lshell a.out lshell_config.lua

lshell: $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJS) $(LFLAGS) -o lshell 
	cp src/lshell_config.lua ./lshell_config.lua
		

lshell.o: $(SRC_DIR)lshell.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: $(SRC_DIR)main.c
	$(CC) $(CFLAGS) -c $< -o $@

commands.o: $(SRC_DIR)commands.c
	$(CC) $(CFLAGS) -c $< -o $@

linux_socket.o: $(SRC_DIR)linux_socket.c
	$(CC) $(CFLAGS) -c $< -o $@

win32_socket.o: $(SRC_DIR)win32_socket.c
	$(CC) $(CFLAGS) -c $< -o $@

lua_interop.o: $(SRC_DIR)lua_interop.c
	$(CC) $(CFLAGS) -c $< -o $@
