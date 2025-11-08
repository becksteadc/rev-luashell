#ifndef cwb_commands_hdr
#define cwb_commands_hdr 1

/* Module model - restricted - TODO - v0.1
* less restricted commands may be available in another module 
*/

/* coupled with the parse_command function */
#define CWB_COMMAND_LEN 4
const char *commands[] = {
	"msg",
	"send",
	"pull",
	"write-txt"
};

enum Commands {
	CMD_MESSAGE = 0, //send message to host machine
	CMD_SEND_DATA, //send data to the C&C machine
	CMD_REQ_DATA, //request data from the host machine
	CMD_DEPOSIT_TXT //store output to a specific text file on the host
};
/* end parse_command helper values */

/* Return is an error code, functionality is in the logic. */
int parse_command(const char *cmd);
int exec_command(enum Commands cmd);

#endif
