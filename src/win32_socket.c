#ifdef OS_WINDOWS

#include <stdio.h>
#include <winsock2.h> //if used with windows.h, must define WIN32_LEAN_AND_MEAN
#include <ws2tcpip.h>

//#include "socket.h" //the same .h as linux sockets should be fine... same interface
#include "win32_socket.h"
#include "commands.h"
#include "definitions.h"
#include "externs.h"

//return is error code
int start_server(void)
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (!result) {
        fprintf(stderr, "%s\n", "Failed to initialize WinSock");
        return -1;
    }
    
    struct addrinfo *adr_info = NULL;
    struct addrinfo socket_flags = {
	.ai_family = AF_INET,
	.ai_socktype = SOCK_STREAM,
	.ai_protocol = IPPROTO_TCP,
        .ai_flags = AI_PASSIVE
    };

    result = getaddrinfo(NULL, GlobalConfig.server_port, &socket_flags, &adr_info);
    if (!result) {
        fprintf(stderr, "%s\n", "Failed to get socket address information");
        WSACleanup();
        return -1;
    }

    SOCKET listen_socket = socket(
        adr_info->ai_family,
        adr_info->ai_socktype,
        adr_info->ai_protocol
    );
    if (listen_socket == INVALID_SOCKET) {
        fprintf(stderr, "%s\n", "Failed to create socket.");
        freeaddrinfo(adr_info);
        WSACleanup();
        return -1;
    }

    result = bind(listen_socket, adr_info->ai_addr, (int) adr_info-> ai_addrlen);
    freeaddrinfo(adr_info); //freed regardless of success or failure
    if (result == SOCKET_ERROR) {
        fprintf(stderr, "%s\n", "Socket bind error.");
        closesocket(listen_socket);
        WSACleanup();
        return -1;
    }

    if (listen(listen_socket, GlobalConfig.max_connections) == SOCKET_ERROR) {
        fprintf(stderr, "%s%ld\n", "Error: ", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return -2;
    }


    return server_handle_conn(listen_socket);
}


int server_handle_conn(SOCKET socket_fd)
{
	#define SERVER_BUFLEN 1024
	char buf[SERVER_BUFLEN];
	struct sockaddr_storage c_addr;
	socklen_t c_addr_len = sizeof(c_addr);
	char quit = 0; //bool flag for the do-while loop
	
	int cfd = accept(socket_fd, (struct sockaddr *) &c_addr, &c_addr_len);
	if (cfd == -1) {
		fprintf(stderr, "%s\n", "failed to accept the reverse binding");
		return -1;
	}	

	if (recv(cfd, buf, sizeof(buf) - 1, 0) > 0) {
		buf[SERVER_BUFLEN - 1] = '\n';
		printf("Contact received: %s\n", buf);
	}
	do {
		quit = server_loop(cfd, buf, sizeof(buf));
	} while (!quit);

	close(cfd);
	close(socket_fd);
	return 0;
}

#endif
//corresponds to #ifdef OS_WINDOWS
