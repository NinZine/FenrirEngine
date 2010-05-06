#include <stdio.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "ifopen.h"
#include "sound.h"
#include "system.h"

extern int luaopen_blender(lua_State* L);
extern int luaopen_event(lua_State* L);
extern int luaopen_image(lua_State* L);
extern int luaopen_mat4(lua_State* L);
extern int luaopen_net(lua_State* L);
extern int luaopen_quat(lua_State* L);
extern int luaopen_render(lua_State* L);
extern int luaopen_sound(lua_State* L);
extern int luaopen_vec3(lua_State* L);

static void l_message (const char *pname, const char *msg);
static int 	report (lua_State *L, int status);

static lua_State *lua_state = 0;

void
l_message (const char *pname, const char *msg)
{
	if (pname) printf("%s: ", pname);
	
	printf("%s\n", msg);
	//fflush(stderr);
}

int
report (lua_State *L, int status)
{
	if (status && !lua_isnil(L, -1)) {
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL) msg = "(error object is not a string)";
		l_message(0, msg);
		lua_pop(L, 1);
	}
	return status;
}

void
sys_quit()
{
	s_quit();
	lua_close(lua_state);
}

int
sys_start(int argc, char *argv[])
{
	int status;
	const char* f;
	lua_State *L;
	
	if (argc<2) {
		printf("%s: <filename.lua>\n",argv[0]);
		return 0;
	}
	
    s_init();
    printf("sound> initialized\n");
	
	L = lua_open();
	/*luaopen_base(L);*/
	luaL_openlibs(L);
	luaopen_mat4(L);
	luaopen_quat(L);
	luaopen_vec3(L);
	luaopen_event(L);
	luaopen_image(L);
	luaopen_render(L);
	luaopen_sound(L);
	luaopen_net(L);
	luaopen_blender(L);
	
	lua_getglobal(L, "package");
	if (LUA_TTABLE != lua_type(L, 1)) {
		printf("lua> 'package' is not a table\n");
		return 1;
	}
	lua_getfield(L, 1, "path");
	if (LUA_TSTRING != lua_type(L, 2)) {
		printf("lua> 'package.path' is not a string\n");
		lua_pop(L, 1);
		return 1;
	}
	lua_pop(L, 1);
	
#if defined(__IPHONE__)
	f = full_path();
#else
	f = ".";
#endif

	/* package.path = f .. "/?.lua" */
	lua_pushlstring(L, f, strlen(f));
	lua_pushliteral(L, "/?.lua");
	lua_concat(L, 2);
	lua_setfield(L, 1, "path");

#if defined(__IPHONE__)
	f = full_path_to_file(argv[1]);
#else
	f = argv[1];
#endif

	printf("lua> initialized\n");
	printf("lua> loading %s\n", f);
	
	if (luaL_loadfile(L,f)==0) { 
		/* function runme = file_content */
		lua_setglobal(L, "runme");
		lua_getglobal(L, "runme");
		status = lua_pcall(L,0,LUA_MULTRET,0);
		report(L, status);
		printf("lua> loaded\n");
	} else {
		printf("lua> unable to load %s\n",f);
		sys_quit();
	}

	lua_state = L;
	
	return status;	
}

int
sys_update()
{
	int status, run;
	
	lua_getglobal(lua_state, "update");
	status = lua_pcall(lua_state,0,LUA_MULTRET,0);
	
	run = 0;
	if (lua_gettop(lua_state) > 0) {
		run = lua_toboolean(lua_state, -1);
	}
	
	report(lua_state, status);
	
	return !run;
}

