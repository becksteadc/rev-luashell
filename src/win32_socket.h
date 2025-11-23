#ifndef cwb_socket_hdr_w32
#define cwb_socket_hdr_w32
#ifdef OS_WINDOWS

/*This header is specific to the windows socket functionality
* linux functionality is in linux_socket.c
*/

//#define CWB_SERVER_PORT 2001 //removed this, defining port in GlobalConfig
// max connections also added to global config
#include <stdio.h>
#include <stddef.h>
#include <winsock2.h> //if used with windows.h, must define WIN32_LEAN_AND_MEAN
#include <ws2tcpip.h>

#include "commands.h"
#include "definitions.h"
#include "externs.h"

int start_server(void);
int server_handle_conn(SOCKET socket_fd);
int server_loop(SOCKET host_conn, char *buf, size_t buflen);


//TODO
//move "client / host" code to separate file, rename to host, not client
int startClient(void);

#endif //OS_WINDOWS
#endif //incl. guard
