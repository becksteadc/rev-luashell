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
	switch (cmd){
		case CMD_MESSAGE:
		break;
		case CMD_SEND_DATA:
		FILE *fp = fopen("lshell-data-out.txt", "w");
		if (fp == NULL) {
			fclose(fp);
			goto error_jump;
		}
		while (recv_file(fp, conn, buf, buflen))
			;
		fclose(fp);
		exec_status = 0;
		break;
		case CMD_REQ_DATA:
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
//two send calls - 1st is file data, 2nd is indicating whether
//the receiver should loop and expect more data to come, or if
//the end has been reached.
//it's kind of essential that they use the same buflen, so maybe that
//should be a constant rather than a passed-in value...
#define XMIT_CONTINUE 0xFFFFFFFF
#define XMIT_END 0xBBBBBBBB
char xmit_file(FILE *fp, SOCKET conn, char *buf, size_t buflen)
{
	int ret_val = 0; //do not continue
	int bytes_read = fread(buf, sizeof(char), buflen, fp);
	if (bytes_read == buflen)
		ret_val = 1;
	send(conn, buf, bytes_read, 0);
	char magic_packet[4];
	if (ret_val)
		memset(magic_packet, 0xFF, 4);//XMIT_CONTINUE
	else
		memset(magic_packet, 0xBB, 4);//XMIT_END
	send(conn, magic_packet, 4, 0); 
	return ret_val;
}

char recv_file(FILE *fp_out, SOCKET conn, char *buf, size_t buflen)
{
	int ret_val = 0;
	int bytes_received = recv(conn, buf, buflen, 0);
	int written = fwrite(buf, sizeof(char), bytes_received, fp_out);
	printf("DEBUG: bytes written in recv_file(): %d\nreceived: %d\n", written, bytes_received);
	if (written != bytes_received) //uh-oh...
		printf("%s\n", "File write error in recv_file()...");
	bytes_received = recv(conn, buf, buflen, 0);
	printf("DEBUG: bytes_received for magic: %d\n", bytes_received);
	unsigned int magic_number = 0;
	for (int i = 0; i < 4; i++) {
		magic_number <<=8;
		magic_number |= buf[i];
		printf("DEBUG: buf[i]: %#x\n", buf[i]);
	}
	printf("DEBUG: magic_number: %#x\n", magic_number);
	if (magic_number == XMIT_CONTINUE)
		ret_val = 1;
	else
		ret_val = 0;

	return ret_val;
}
//guarantee these are not used elsewhere for mental clarity
#undef XMIT_CONTINUE
#undef XMIT_END

int exec_command_server_side(SOCKET conn, enum Commands cmd, char *buf, size_t buflen)
{
	int exec_status = -1;
	switch (cmd) {
		case CMD_MESSAGE:
		break;
		case CMD_SEND_DATA:
		printf("Filename to send:\n");
		char fname[64] = { 0 };
		scanf_s("%s", &fname, 64);
		FILE *fp = fopen(fname, "r");
		if (fp == NULL) {
			fclose(fp);
			exec_status = 1;
			break;
		}
		//fread(buf, sizeof(char), buflen, fp);
		while (xmit_file(fp, conn, buf, buflen) != 0)
			;//good spot to check for infinite loops if that problem comes up
		fclose(fp);
		exec_status = 0;
		break;
		case CMD_REQ_DATA:
		break;
		case CMD_DEPOSIT_TXT:
		break;
		default:
		return -1;
	}
	return exec_status; //set to return value of command execution in switch block
}
