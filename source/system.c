#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "log.h"
#include "ifopen.h"
#include "sound.h"
#include "system.h"

//extern int luaopen_blender(lua_State* L);
extern int luaopen_event(lua_State* L);
extern int luaopen_font(lua_State* L);
extern int luaopen_image(lua_State* L);
extern int luaopen_mat4(lua_State* L);
extern int luaopen_md2(lua_State* L);
extern int luaopen_net(lua_State* L);
extern int luaopen_quat(lua_State* L);
extern int luaopen_render(lua_State* L);
extern int luaopen_sound(lua_State* L);
extern int luaopen_sys(lua_State* L);
extern int luaopen_log(lua_State* L);
extern int luaopen_vec3(lua_State* L);
extern int luaopen_util(lua_State* L);

static void l_message (const char *pname, const char *msg);
static int 	report (lua_State *L, int status);
static void lua_bootstrap(lua_State *L, const char *path);

static lua_State *lua_state = 0;

void
l_message (const char *pname, const char *msg)
{
	if (pname) printf("%s: ", pname);
	
	printf("lua> %s\n", msg);
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
lua_bootstrap(lua_State *L, const char *path)
{
	const char *f = "lib_lua/bootstrap.lua";
	int status;

	/* Load lib_lua/bootstrap.lua */
#if defined(__IPHONE__)
	f = full_path_to_file(f);
#endif

	if (0 != (status = luaL_dofile(L, f))) {
		log_print("lua> could not bootstrap\n");
	}

	assert(0 == report(L, status));
}

void
sys_exec_string(const char *str)
{
	int success;
	
	log_print(str);
	log_print("\n");
	success = luaL_dostring(lua_state, str);
	if (success) {
		log_print("lua> error executing string\n");
	}
}

void
sys_quit()
{
	s_quit();
	if (lua_state) {
		int kb = lua_gc(lua_state, LUA_GCCOUNT, 0);
		lua_close(lua_state);

		printf("lua> %d kB used\n", kb);
	}
}

int
sys_start(int argc, char *argv[])
{
	int status;
	const char* f;
	lua_State *L;
	
	if (argc<2) {
		printf("%s: <filename.lua>\n", argv[0]);
		return 1;
	}
	
    s_init();
    log_print("sound> initialized\n");
	
	L = lua_open();
	/*luaopen_base(L);*/
	luaL_openlibs(L);
	luaopen_sys(L);
	luaopen_log(L);
	luaopen_mat4(L);
	luaopen_quat(L);
	luaopen_vec3(L);
	luaopen_event(L);
	luaopen_image(L);
	luaopen_render(L);
	luaopen_sound(L);
	luaopen_net(L);
	luaopen_util(L);
	luaopen_font(L);
	luaopen_md2(L);
	//luaopen_blender(L);
	
	lua_getglobal(L, "package");
	if (LUA_TTABLE != lua_type(L, 1)) {
		log_print("lua> 'package' is not a table\n");
		return 1;
	}
	lua_getfield(L, 1, "path");
	if (LUA_TSTRING != lua_type(L, 2)) {
		log_print("lua> 'package.path' is not a string\n");
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

	lua_bootstrap(L, f);

#if defined(__IPHONE__)
	f = full_path_to_file(argv[1]);
#else
	f = argv[1];
#endif

	log_print("lua> initialized\n");
	printf("lua> loading %s\n", f);
	
	if (luaL_loadfile(L,f)==0) { 
		/* function runme = file_content */
		lua_setglobal(L, "runme");
		lua_getglobal(L, "runme");

		status = lua_pcall(L,0,LUA_MULTRET,0);
		assert(0 == report(L, status));
        
        if (lua_gettop(L) > 0) {
            status = lua_toboolean(L, -1);
            if (!status) {
                printf("lua> %s returned false\n", f);
                sys_quit();
            }
        }
		
        log_print("lua> loaded\n");
	} else {
		printf("lua> unable to load %s\n",f);
		report(L, status);
		sys_quit();
	}

	lua_state = L;
	
	/* TODO: Double check this stuff some day when bored. */
	return status > 0 ? false : true;	
}

int
sys_update()
{
	int status, run;
	
	lua_getglobal(lua_state, "update");
	if (lua_gettop(lua_state) > 0) {
	    status = lua_pcall(lua_state,0,LUA_MULTRET,0);
    } else {
		lua_getglobal(lua_state, "runme");
		status = lua_pcall(lua_state,0,LUA_MULTRET,0);
    }
	
	run = 0;
	if (lua_gettop(lua_state) > 0) {
		run = lua_toboolean(lua_state, -1);
	}
	
	assert(0 == report(lua_state, status));
	
	return !run;
}

