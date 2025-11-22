//TODO - have this file handle all the lua
// namely, it should read in the definitions in the lua file
//and use them as configuration settings passed to the rest of the program
#include <stdio.h>
//TODO - very nonportable includes here...
#include "../lua-5.4.8/src/lua.h"
#include "../lua-5.4.8/src/lauxlib.h"
#include "../lua-5.4.8/src/lualib.h"

#include "definitions.h"
#include "lua_interop.h"


//global definition
struct ProgramConfig GlobalConfig;

/* Negative return values are considered errors.
*/
int getglobint(lua_State *L, const char *var)
{
	int isnum, result;
	lua_getglobal(L, var);
	result = (int) lua_tointegerx(L, -1, &isnum);
	if (!isnum) {
		printf("Error: %s should be a number.\n", var);
		return -1;
	}
	lua_pop(L, 1);
	return result;
}



/* Returns an error code value: nonzero is error. 
* Loads the necessary config values into GlobalConfig from the lua program
*/
int lshell_load_state(void)
{
    lua_State *l_state = luaL_newstate();
    luaL_openlibs(l_state);
    if (luaL_loadfile(l_state, CONFIG_FILENAME) || lua_pcall(l_state, 0, 0, 0)) {
        printf("%s\n", "Error: failed to open lua config file: CONFIG_FILENAME");
        return -1;
    }

    //TODO - use STATE_VARIABLES rather than hardcoded strings
    if ((GlobalConfig.server_port = getglobint(l_state, "server_port")) == -1)
       return -1; //error - was unable to read the value needed
    if ((GlobalConfig.max_connections = getglobint(l_state, "server_port")) == -1)
        return -1; //error - was unable to read the value needed

    return 0;
}
