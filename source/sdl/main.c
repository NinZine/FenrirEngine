#include <stdbool.h>
#include <stdio.h>
#include <SDL/SDL.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern int luaopen_blender(lua_State* L);
extern int luaopen_event(lua_State* L);
extern int luaopen_image(lua_State* L);
extern int luaopen_mat4(lua_State* L);
extern int luaopen_net(lua_State* L);
extern int luaopen_quat(lua_State* L);
extern int luaopen_render(lua_State* L);
extern int luaopen_vec3(lua_State* L);

static void
l_message (const char *pname, const char *msg)
{
	if (pname) printf("%s: ", pname);
	
	printf("%s\n", msg);
	//fflush(stderr);
}

static int
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

static bool 
sdl_init()
{

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("sdl> failed SDL_INIT_EVERYTHING\n");
		return false;
	}
	//freopen( "CON", "w", stdout );
	//freopen( "CON", "w", stderr );

	SDL_EnableUNICODE(1);
	return true;
}

int main(int argc,char* argv[])
{
	lua_State *L = 0;
	int status;
	int *a = 0;

	if (argc<2) {
		printf("%s: <filename.lua>\n",argv[0]);
		return 0;
	}

	printf("sdl> init\n");
	if (false == sdl_init()) {
		printf("sdl> failed to initialize\n");
		return 0;
	}
	printf("sdl> initialized\n");

	L=lua_open();
	/*luaopen_base(L);*/
	luaL_openlibs(L);
	luaopen_mat4(L);
	luaopen_quat(L);
	luaopen_vec3(L);
	luaopen_event(L);
	luaopen_image(L);
	luaopen_render(L);
	luaopen_net(L);
	luaopen_blender(L);
	
	printf("lua> initialized\n");
	printf("lua> loading %s\n",argv[1]);
	
	if (luaL_loadfile(L,argv[1])==0) { // load and run the file
		status = lua_pcall(L,0,LUA_MULTRET,0);
		report(L, status);
	} else {
		printf("unable to load %s\n",argv[1]);
	}
	lua_close(L);

	SDL_Quit();
	return status;
}

