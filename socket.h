#ifndef cwb_socket_hdr
#define cwb_socket_hdr

#include <stddef.h>

//#define CWB_SERVER_PORT 2001 //removed this, defining port in GlobalConfig

int start_server(void);
int server_handle_conn(int socket_fd);
int server_loop(int host_conn, char *buf, size_t buflen);


//TODO
//move "client / host" code to separate file, rename to host, not client
int startClient(void);


#endif
