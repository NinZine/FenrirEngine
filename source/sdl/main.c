#include <stdbool.h>
#include <stdio.h>

#include <SDL/SDL.h>

#include "system.h"


static bool 
sdl_init()
{

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("sdl> failed SDL_INIT_EVERYTHING\n");
		return false;
	}
	//freopen( "CON:", "w", stdout );
	//freopen( "CON:", "w", stderr );

	SDL_EnableUNICODE(1);
	return true;
}

int main(int argc,char* argv[])
{
	int status;

	printf("sdl> init\n");
	if (false == sdl_init()) {
		printf("sdl> failed to initialize\n");
		return 0;
	}
	printf("sdl> initialized\n");

    status = sys_start(argc, argv);
	while (0 == status) {
		status = sys_update();
	}
	
    sys_quit();
	SDL_Quit();
	
	return !status;
}

