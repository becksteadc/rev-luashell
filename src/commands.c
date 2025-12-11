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
