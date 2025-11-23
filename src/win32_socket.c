#ifdef OS_WINDOWS

#include "win32_socket.h"

//return is error code
int start_server(void)
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
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

		char port[16]; //windows expects the port # as a string for some reason
		snprintf(port, 16, "%d", GlobalConfig.server_port);

	    result = getaddrinfo(NULL, port, &socket_flags, &adr_info);
	    if (result != 0) {
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
			fprintf(stderr, "%s%d\n", "Error: ", WSAGetLastError());
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
	//client file descriptor = cfd
	SOCKET cfd = accept(socket_fd, (struct sockaddr *) &c_addr, &c_addr_len);
	closesocket(socket_fd);

	if (cfd == INVALID_SOCKET) {
		fprintf(stderr, "%s\n", "failed to accept the reverse binding");
		closesocket(cfd);
		WSACleanup();
		return -1;
	}	

	if (recv(cfd, buf, sizeof(buf) - 1, 0) > 0) {
		buf[SERVER_BUFLEN - 1] = '\n';
		printf("Contact received: %s\n", buf);
	}
	do {
		quit = server_loop(cfd, buf, sizeof(buf));
	} while (!quit);

	closesocket(cfd);
	return 0;
}


//contact is successfully made, so this handles each message.
//return nonzero to quit the loop
int server_loop(SOCKET host_conn, char *buf, size_t buflen)
{
	char command[1]; //hack...
	
	printf("%s\n", "Enter command to send: ");
	scanf("%s", buf); 
	buf[buflen - 1] = '\0';
	command[0] = parse_command(buf); 
	if (command[0] == CMD_QUIT)
		return 1;

	int send_status = send(host_conn, command, sizeof(int), 0);
	if (send_status == SOCKET_ERROR)
		printf("%s\n", "Failed to send command over socket.");
	else {
		for (int i = 0; i < buflen; i++) { //memset buffer to zero but better?
			if (buf[i] == '\0') break;
			buf[i] = 0;
		}
		int response = recv(host_conn, buf, buflen - 1, 0);
		if (response > 0) {
			buf[buflen - 1] = '\0';
			printf("Return response: %s\n", buf);
		}
	}
	return 0;
}

#endif
//corresponds to #ifdef OS_WINDOWS
