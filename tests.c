#include <stdio.h>

#include "definitions.h"
#include "externs.h"
#include "lua_interop.h"

/* test main - not built in the normal program
*/
int main()
{
	if (lshell_load_state()) {
		printf("Encountered error...\n");
		return -1;
	}
	printf("Global config read value: %d\n", GlobalConfig.server_port);
}
