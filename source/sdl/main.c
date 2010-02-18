#include <stdio.h>
#include <SDL/SDL.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern int luaopen_event(lua_State* L);
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

static SDL_Surface*
sdl_init()
{
	SDL_Surface *surface;
	const SDL_VideoInfo *info;
	int flags = 0;
	uint8_t depth = 0;


	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("sdl> failed SDL_INIT_EVERYTHING\n");
	}
	//freopen( "CON", "w", stdout );
	//freopen( "CON", "w", stderr );

	info = SDL_GetVideoInfo();
	depth = info->vfmt->BitsPerPixel;
	flags = SDL_OPENGL;

	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1))
	    printf("sdl> no double buffer\n");
	if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8))
	    printf("sdl> no depth buffer\n");
	if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8))
	    printf("sdl> no stencil buffer\n");
	if (SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1))
	    printf("sdl> no swapping\n");

	surface = SDL_SetVideoMode(800, 600, depth, flags);
	if (!surface) {
	    printf("sdl> no video\n");
	    exit(1);
	}

	SDL_EnableUNICODE(1);
	return surface;
}

int main(int argc,char* argv[])
{
	SDL_Surface *s = 0;
	lua_State *L = 0;
	int status;
	int *a = 0;

	if (argc<2) {
		printf("%s: <filename.lua>\n",argv[0]);
		return 0;
	}

	printf("sdl> init\n");
	s = sdl_init();
	printf("sdl> initialized\n");

	L=lua_open();
	/*luaopen_base(L);*/
	luaL_openlibs(L);
	luaopen_mat4(L);
	luaopen_quat(L);
	luaopen_vec3(L);
	luaopen_event(L);
	luaopen_render(L);
	luaopen_net(L);

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

