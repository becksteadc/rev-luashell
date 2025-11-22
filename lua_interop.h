#ifndef cbw_lua_interop_h
#define cwb_lua_interop_h 1

int lshell_load_state(void);
#define CONFIG_FILENAME "lshell_config.lua"

//fine to have as a string array, since these are only parsed once (no hash)
#define STATE_VARIABLES_COUNT 3
const char *STATE_VARIABLES[3] = {
    "server_port",
    "backup_port",
    "max_connections"
};

#endif
