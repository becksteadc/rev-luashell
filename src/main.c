//Orca was here
#include <stdio.h>
#include <string.h>

#ifdef OS_WINDOWS
#include "win32_socket.h"
#endif

#ifdef OS_LINUX
#include "linux_socket.h"
#endif

#include "lua_interop.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s < server | client >\n", argv[0]);
		return -1;
	}

	lshell_load_state();
	if (strcmp(argv[1], "server") == 0) {
		printf("Global config port: %d\n", GlobalConfig.server_port);
		start_server();
	} else if (strcmp(argv[1], "client") == 0) {
		printf("Launching client\n");
		start_host();
	} else {
		printf("%ld\n", __STDC_VERSION__);
		printf("Project build successful.");
		getchar();
	}
	return 0;
}
