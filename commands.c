/* TODO - v0.1
* This file should be a module defining the commands compiled into the shell.
* It should be replacible for different use cases. Notably, when distributed,
* it should be given only limited "proof of concept" commands that are unlikely
* to cause system harm or be used for abuse.
*
* Module model - restricted
*/
#include <stdio.h>
#include <string.h>
#include "commands.h"

const char *command_strings[] = {
	"quit",
	"msg",
	"send",
	"pull",
	"write-txt"
};

/* return the int corresponding to the command to execute */
int parse_command(const char *cmd)
{
	for (int i = 0; i < CWB_COMMAND_LEN; i++) {
		/* const char *commands[] is defined in commands.h */
		if (strcmp(cmd, command_strings[i]) == 0) {
			return i; //i is guaranteed to match the enum... cast the result?
		}
	}
	fprintf(stderr, "%s\n", "FATAL: failed to parse command");
	return -1; //clean up and exit program
}

/*
* See definition of the commands enum in commands.h
* This should be run on the "client" / controlled side.
*/ 
int exec_command(enum Commands cmd) //TODO exec commands...
{
	int exec_status = -1;
	switch (cmd){
		case CMD_MESSAGE:
		break;
		case CMD_SEND_DATA:
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
