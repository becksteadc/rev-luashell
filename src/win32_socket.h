#ifndef cwb_socket_hdr
#define cwb_socket_hdr

/*This header is specific to the windows socket functionality
* linux functionality is in linux_socket.c
*/



#include <stddef.h>

//#define CWB_SERVER_PORT 2001 //removed this, defining port in GlobalConfig
// max connections also added to global config

int start_server(void);
int server_handle_conn(SOCKET socket_fd);
//TODO probably have to change server_loop function prototype too (linux->windows rewrite)
int server_loop(int host_conn, char *buf, size_t buflen);


//TODO
//move "client / host" code to separate file, rename to host, not client
int startClient(void);


#endif
