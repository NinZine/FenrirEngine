#include <SDL/SDL.h>

#include "event.h"

static event event_convert(SDL_Event *e);

event
event_convert(SDL_Event *e)
{
    event t;

    t.type = EMPTY;
    switch (e->type) {
        case SDL_KEYDOWN:
            t.type = KEYDOWN;
            t.key.type = KEYDOWN;
            t.key.c = e->key.keysym.unicode;
			t.key.sym = e->key.keysym.sym;
			break;
        case SDL_KEYUP:
            t.type = KEYUP;
            t.key.type = KEYUP;
            t.key.c = e->key.keysym.unicode;
			t.key.sym = e->key.keysym.sym;
			break;
    }

    return t;
}

event
event_poll()
{
    SDL_Event e;

    /*SDL_PumpEvents();*/
    SDL_PollEvent(&e);
    return event_convert(&e);
}

void
event_sleep(unsigned int ms)
{
    SDL_Delay(ms);
}

