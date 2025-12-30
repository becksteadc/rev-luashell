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
	struct addrinfo socket_flags;
	ZeroMemory(&socket_flags, sizeof(socket_flags));
	socket_flags.ai_family = AF_INET;
	socket_flags.ai_socktype = SOCK_STREAM;
	socket_flags.ai_protocol = IPPROTO_TCP;
	socket_flags.ai_flags = AI_PASSIVE;

	//char port[16]; //windows expects the port # as a string for some reason
	//snprintf(port, 16, "%d", GlobalConfig.server_port);
	//port[15] = '\0';
	//printf("snprintf ver: %s htons ver: %d\n", port, htons(2001));	
	#define TEMP_DEFAULT_PORT "2001"

	result = getaddrinfo(NULL, TEMP_DEFAULT_PORT, &socket_flags, &adr_info);
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

	result = bind(listen_socket, adr_info->ai_addr, (int) adr_info->ai_addrlen);
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

	//server_handle_conn now owns listen_socket and closes it
	int connection_result = server_handle_conn(listen_socket);
	WSACleanup();
	return connection_result;
}


int server_handle_conn(SOCKET socket_fd)
{
	#define SERVER_BUFLEN 1024
	char buf[SERVER_BUFLEN];
	//struct sockaddr_storage c_addr;
	//socklen_t c_addr_len = sizeof(c_addr);
	char quit = 0; //bool flag for the do-while loop
	//cfd -- client file descriptor
	//fprintf(stdout, "DEBUG: reached accept() call\n");
	SOCKET cfd = accept(socket_fd, NULL, NULL);
	closesocket(socket_fd);

	if (cfd == INVALID_SOCKET) {
		fprintf(
			stderr,
			"%s%d\n",
			"failed to accept the reverse binding. Err: ",
			WSAGetLastError()
		);
		WSACleanup();
		return -1;
	}	

	//fprintf(stderr, "DEBUG: reached recv()\n");
	int result = recv(cfd, buf, sizeof(buf), 0);
	if (result == SOCKET_ERROR) {
		printf("send failed with error on recv(): %d", WSAGetLastError());
		closesocket(cfd);
		WSACleanup();
		return -2;
	}
	buf[SERVER_BUFLEN - 1] = '\0';
	printf("Contact received: %s\n", buf);
	
	do {
		//fprintf(stderr, "DEBUG: reached server_loop begin\n");
		quit = server_loop(cfd, buf, sizeof(buf));
		//fprintf(stderr, "DEBUG: reached server_loop exit\n");
	} while (!quit);

	closesocket(cfd);
	return 0;
}


//contact is successfully made, so this handles each message.
//return nonzero to quit the loop
char server_loop(SOCKET host_conn, char *buf, size_t buflen)
{
	char command[1]; //hack...
	char ret_val = 0;

	printf("%s\n", "Enter command to send: ");
	scanf("%s", buf); 
	buf[buflen - 1] = '\0';
	command[0] = parse_command(buf); 
	if (command[0] == CMD_QUIT)
		return 1;

	int send_status = send(host_conn, command, sizeof(char), 0);
	if (send_status == SOCKET_ERROR) {
		printf("%s\n", "Failed to send command over socket.");
		ret_val = 1;
	}
	else {
		ret_val = exec_command_server_side(host_conn, command[0], buf, buflen);
		
	}
	return ret_val;
}

int start_host()
{
	#define CLIENT_BUFLEN 256
	struct addrinfo *address_result, address_flags, *ptr;
	address_result = NULL;
	ptr = NULL;
	ZeroMemory(&address_flags, sizeof(address_flags));
	address_flags.ai_protocol = IPPROTO_TCP;
	address_flags.ai_socktype = SOCK_STREAM;
	address_flags.ai_family = AF_UNSPEC;

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		fprintf(stderr, "%s\n", "Failed to initialize WinSock");
		return -1;
	}
	//DEBUG TODO - switch to lua config later
	#define SERVER_HOSTNAME "localhost"
	//#define SERVER_HOSTNAME "127.0.0.1"
	#define DEFAULT_PORT_DEBUG "2001"
	result = getaddrinfo(SERVER_HOSTNAME, DEFAULT_PORT_DEBUG, &address_flags, &address_result);
	if (result != 0) {
		fprintf(stderr, "failed to get server address info, code: %d\n", result);
		WSACleanup();
		return -1;
	}
	SOCKET client_socket;	
	for (ptr = address_result; ptr != NULL; ptr = ptr->ai_next) {
		client_socket = socket(
			ptr->ai_family,
			ptr->ai_socktype,
			ptr->ai_protocol);
		if (client_socket == INVALID_SOCKET) {
			fprintf(stderr, "call to socket() failed, code: %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}
		result = connect(client_socket, ptr->ai_addr, (int) ptr->ai_addrlen);
		if (result == SOCKET_ERROR) {
			fprintf(stderr, "Failed to connect socket to server, code: %d\n", WSAGetLastError());
			closesocket(client_socket);
			client_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	
	freeaddrinfo(address_result);
	
	if (client_socket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	char buf[CLIENT_BUFLEN];	
	result = send(client_socket, "Testing send", sizeof("Testing send"), 0);
	if (result == SOCKET_ERROR) {
		fprintf(stderr, "send failed, error: %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return -3;
	}
	printf("%s\n", "Contact established, listening.");
	result = 1;
	//TODO - stop client from exiting after only a couple of commands
	char quit_counter = 2;
	do {
		memset(buf, 0, CLIENT_BUFLEN);
		result = recv(client_socket, buf, CLIENT_BUFLEN, 0);
		if (result == 0)
			quit_counter--;
		else
			quit_counter = 2;
		if (result == SOCKET_ERROR) {
			fprintf(stderr, "socket error %d\n", WSAGetLastError());
			closesocket(client_socket);
			WSACleanup();
			return -4;
		}
		//buf[result] = '\0';
		//printf("%s\n", buf);	
		printf("Result len: %d\n", result);
		for (int i = 0; i < result; i++) {
			printf("Result:\t%c\tHex: %#X\n", buf[i], buf[i]);
		}
		exec_command(buf[0], client_socket, buf, CLIENT_BUFLEN);//TODO temp, testing, remove, delete later, etc
	} while(quit_counter);

	closesocket(client_socket);
	WSACleanup();
	return 0;
}

#endif
//corresponds to #ifdef OS_WINDOWS
