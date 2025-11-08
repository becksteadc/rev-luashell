#ifndef cwb_socket_hdr
#define cwb_socket_hdr 1

#include <stddef.h>

#define CWB_SERVER_PORT 2001

int start_server(void);
int server_handle_conn(int socket_fd);
int server_loop(int host_conn, char *buf, size_t buflen);


//TODO
//move "cliend / host" code to separate file, rename to host, not client
int startClient(void);


#endif
