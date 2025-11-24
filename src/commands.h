#ifndef cwb_commands_hdr
#define cwb_commands_hdr

/* Module model - restricted - TODO - v0.1
 * less restricted commands may be available in another module
 */

/* coupled with the parse_command function */
#define CWB_COMMAND_LEN 5

extern const char *command_strings[];

enum Commands {
  CMD_QUIT = 0,
  CMD_MESSAGE,    // send message to host machine
  CMD_SEND_DATA,  // send data to the C&C machine
  CMD_REQ_DATA,   // request data from the host machine
  CMD_DEPOSIT_TXT // store output to a specific text file on the host
};

struct Hashmap {
  char *key;           // This is the command as a str
  enum Commands value; // This is the enum corresponding to that str
};
/* end parse_command helper values */

/* Return is an error code, functionality is in the logic. */
int parse_command(const char *cmd);
int exec_command(enum Commands cmd);
struct Hashmap *create_hashmap();
int parse_command_hashmap(const char *cmd);

#endif
