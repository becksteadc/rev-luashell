#ifndef cwb_socket_hdr_linux
#define cwb_socket_hdr_linux
#ifdef OS_LINUX
/*This header is specific to the linux socket functionality (currently socket.c)
* socket.c will eventually be renamed to linux_socket.c
*/


#include <stdio.h>
#include <stddef.h>
#include <sys/socket.h>
#include <netdb.h> //for little to big endian
#include <stdlib.h>
#include <string.h> //slated for removal? only used in client-side code
#include <arpa/inet.h> //for inet_pton()
#include <unistd.h>

#include "commands.h"
#include "definitions.h"
#include "externs.h"


//#define CWB_SERVER_PORT 2001 //removed this, defining port in GlobalConfig
// max connections also added to global config

int start_server(void);
int server_handle_conn(int socket_fd);
int server_loop(int host_conn, char *buf, size_t buflen);


int startHost(void);

#endif //OS_LINUX
#endif //incl. guard
