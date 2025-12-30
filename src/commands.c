/* TODO - v0.1
* This file should be a module defining the commands compiled into the shell.
* It should be replacible for different use cases. Notably, when distributed,
* it should be given only limited "proof of concept" commands that are unlikely
* to cause system harm or be used for abuse.
*
* Module model - restricted
*/

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "commands.h"




/* coupled with the parse_command function */
#define CWB_COMMAND_LEN 5
const char *command_strings[] = {
	"quit",
	"msg",
	"send",
	"pull",
	"write-txt"
};

/* creates hashmap where the commands as string are the key and the enum are the value*/
struct Hashmap *create_hashmap()
{
    struct Hashmap *hashmap = NULL;
    for(int i = 0; i < CWB_COMMAND_LEN; i++){
        //This line sets the key and value of the hashmap where i is assumed to be the corresponding enum
        shput(hashmap, (char *)command_strings[i], i);
    }
    shdefault(hashmap, -1); //if given a key that is not in the hashmap it will return -1 meaning error
    return hashmap;
}


/* return the enum of the command given a str using a hashmap*/
int parse_command(const char *cmd){
	static struct Hashmap *hashmap = NULL;
	if (hashmap == NULL)
		hashmap = create_hashmap();
		
    return shget(hashmap, cmd);
}


/*
* See definition of the commands enum in commands.h
* exec_command should be run on the "client" / controlled side.
* SOCKET, while normally windows only, will be used here to make the code
* properly portable.
* For security, server and client binaries should actually be different
* programs... but this is POC only.
*
* I need some sort of network protocol for the client and server to be
* able to negotiate properly: when the server says CMD_SEND_DATA, the client
* gets to the exec_command switch branch for that command, then needs request
* the actual file to write to disk. 
*/

#ifdef OS_LINUX
typedef int SOCKET;
#endif
int exec_command(enum Commands cmd, SOCKET conn, char *buf, size_t buflen) //TODO exec commands...
{
	int exec_status = -1;
	char fname[64] = { 0 };
	FILE *fp = NULL;
	switch (cmd){
		case CMD_MESSAGE:
		break;
		case CMD_SEND_DATA:
		fp = fopen("lshell-data-out.txt", "w");
		if (fp == NULL) {
			//fclose(fp); // shouldn't close NULL file pointer...
			goto error_jump;
		}
		while (recv_file(fp, conn))
			;
		fclose(fp);
		exec_status = 0;
		break;
		case CMD_REQ_DATA:
		int bytes_received = recv(conn, fname, 64, 0);
		if (bytes_received == 0) {
			printf("Received 0 bytes for filename in req_data clientside.");
			return 1;
		}
		fp = fopen(fname, "r");
		if (fp == NULL) {
			goto error_jump;
		}
		while (xmit_file(fp, conn) != 0)
			;
		fclose(fp);
		exec_status = 0;
		break;
		case CMD_DEPOSIT_TXT:
		break;
		default:
		return -1;
	}
	return exec_status; //set to return value of command execution in switch block

	error_jump:
	send(conn, "Execution failure", sizeof("Execution failure"), 0);
	return -1;
}

//return code is boolean: whether to continue looping calls to this
//function or not.
//does not close *fp or conn
//it's kind of essential that they use the same buflen, so maybe that
//should be a constant rather than a passed-in value...
#define XMIT_CONTINUE 0xFFFFFFFF
#define XMIT_END 0xBBBBBBBB
#define PACKET_FLAG_LEN 4
#define XMIT_BUFFER_LEN 1024
char xmit_file(FILE *fp, SOCKET conn)
{
	int ret_val = 0; //do not continue
	char buf[XMIT_BUFFER_LEN] = {0};
	int bytes_read = fread(buf, sizeof(char), XMIT_BUFFER_LEN - PACKET_FLAG_LEN, fp);
	if (bytes_read == XMIT_BUFFER_LEN - PACKET_FLAG_LEN)
		ret_val = 1;

	if (ret_val)
		memset(buf + bytes_read, 0xFF, 4);//XMIT_CONTINUE
	else
		memset(buf + bytes_read, 0xBB, 4);//XMIT_END
	send(conn, buf, bytes_read + PACKET_FLAG_LEN, 0);
	return ret_val;
}

char recv_file(FILE *fp_out, SOCKET conn)
{
	int ret_val = 0;
	char buf[XMIT_BUFFER_LEN] = {0};
	int bytes_received = recv(conn, buf, XMIT_BUFFER_LEN, 0);
	uint32_t magic_number = 0;
	for (int i = 0; i < PACKET_FLAG_LEN; ++i) {
		magic_number <<= 8;
		magic_number |= (unsigned char) buf[i + bytes_received - PACKET_FLAG_LEN];
	}

	if (magic_number == XMIT_CONTINUE)
		ret_val = 1;
	else if (magic_number == XMIT_END)
		ret_val = 0;
	else {
		ret_val = 2;
		printf("recv_file did not receive a proper packet flag. File integrity compromised.\n");
	}

	int written = fwrite(buf, sizeof(char), bytes_received - PACKET_FLAG_LEN, fp_out);
	printf("DEBUG: bytes written in recv_file(): %d\nreceived: %d\n", written, bytes_received);
	//expect this to be PACKET_FLAG_LEN less than the received value
	fflush(stdout);
	if (written != bytes_received - PACKET_FLAG_LEN) //uh-oh...
		printf("%s\n", "File write error in recv_file()...");

	return ret_val;
}
//guarantee these are not used elsewhere for mental clarity
#undef XMIT_CONTINUE
#undef XMIT_END
#undef PACKET_FLAG_LEN
#undef XMIT_BUFFER_LEN

int exec_command_server_side(SOCKET conn, enum Commands cmd, char *buf, size_t buflen)
{
	int exec_status = -1;
	char fname[64] = { 0 };
	FILE *fp = NULL;
	switch (cmd) {
		case CMD_MESSAGE:
		break;
		case CMD_SEND_DATA:
		printf("Filename to send:\n");
		scanf_s("%s", &fname, 64);
		fp = fopen(fname, "r");
		if (fp == NULL) {
			fclose(fp);
			exec_status = 1;
			break;
		}
		//fread(buf, sizeof(char), buflen, fp);
		while (xmit_file(fp, conn) != 0)
			;//good spot to check for infinite loops if that problem comes up
		fclose(fp);
		exec_status = 0;
		break;
		case CMD_REQ_DATA:
		printf("Filename to request:\n");
		scanf_s("%s", &fname, 64);
		send(conn, fname, 64, 0);
		printf("Filename to (over)write:\n");
		scanf_s("%s", &fname, 64);
		fp = fopen(fname, "w");
		if (fp == NULL) {
			fclose(fp);
			exec_status = 1;
			break;
		}
		while (recv_file(fp, conn) != 0)
			;
		fclose(fp);
		exec_status = 0;
		break;
		case CMD_DEPOSIT_TXT:
		break;
		default:
		printf("Not recognized as an internal or external command, operable program, or batch file.");
		return -1;
	}
	if (exec_status == -1) exec_status = 0; //TODO debug - prevent server from always closing conn
	return exec_status; //set to return value of command execution in switch block
}
