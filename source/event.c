#include <sys/time.h>

#include <stdint.h>
#include <stdio.h>

#if defined(__SDL__)
# include <SDL/SDL.h>
#elif defined(__NDS__)
# include <nds/nds.h>
# include <nds/timers.h>
#endif

#include "event.h"
#include "log.h"


#if defined(__SDL__)
static uint8_t warp = 0;
static event event_convert(SDL_Event *e);
#endif

static event queue[20];
static uint8_t events = 0;

#if defined(__SDL__)
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
		case SDL_MOUSEBUTTONDOWN:
			t.type = TOUCHDOWN;
			t.touch.type = TOUCHDOWN;
			t.touch.dx = e->button.x;
			t.touch.dy = e->button.y;
			break;
		case SDL_MOUSEBUTTONUP:
			t.type = TOUCHUP;
			t.touch.type = TOUCHUP;
			t.touch.dx = e->button.x;
			t.touch.dy = e->button.y;
			break;
		case SDL_MOUSEMOTION:
			if (warp) {
				warp = 0;
				break;
			}
			//if (e->motion.state) {
			t.type = TOUCHDOWN;
			t.touch.type = TOUCHDOWN;
			t.touch.dx = e->motion.x;
			t.touch.dy = e->motion.y;
			t.touch.sx = e->motion.x - e->motion.xrel;
			t.touch.sy = e->motion.y - e->motion.yrel;
			//}
			break;
    }

    return t;
}
#endif

event
event_poll()
{
#if defined(__SDL__)
    SDL_Event e;

    /*SDL_PumpEvents();*/
    SDL_PollEvent(&e);
    return event_convert(&e);
#else
	event ev;
	ev.type = EMPTY;
	if (events > 0) {
		events--;
		ev = queue[events];
	}
	
	return ev;
#endif
}

void
event_push(event e)
{	
	queue[events] = e;
	events = events + 1;
	if (events >= 20) {
		log_printf("event> more than 20 events");
		events = 19;
	}
}

void
event_show_cursor(uint8_t show)
{
#if defined(__SDL__)
	int x, y;
	SDL_GetMouseState(&x, &y);
	SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
	event_warp_mouse(x, y);
#endif
}

void
event_sleep(uint32_t ms)
{
#if defined(__SDL__)
    SDL_Delay(ms);
#endif
}

uint32_t
event_time()
{
#if defined(WIN32)

	return SDL_GetTicks();

#elif !defined(__NDS__)
	static struct timeval start = {0,0};
	struct timeval counter;
	uint32_t tmp;
	
	if (0 == start.tv_sec && 0 == start.tv_usec) {
		gettimeofday(&start, 0);
		return 0.f;
	}
	
	gettimeofday(&counter, 0);
	counter.tv_sec = counter.tv_sec - start.tv_sec;
	counter.tv_usec = counter.tv_usec - start.tv_usec;
	
	tmp = ((counter.tv_sec * 1000000) + counter.tv_usec) / 1000;
    return tmp;

#else /* !__IPHONE__ */
	static struct timeval counter = {0,0};
	static bool started = false;
	uint16_t tmp = 0;

	if (false == started) {
		timerStart(0, ClockDivider_1, 0, 0);
		
		//TIMER0_DATA = 0; //0xFFFF - 32768; 
		//TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
		started = true;
	}


	tmp = timerElapsed(0) / 33;
	counter.tv_usec += (tmp >= 993) ? tmp / 2 : tmp; //TIMER0_DATA / 33;
	while (counter.tv_usec > 100000) {
		counter.tv_sec += 1;
		counter.tv_usec -= 100000;
	}

	//printf("sec: %d ", counter.tv_sec);
	//printf("usec: %d\n", counter.tv_usec);
	return ((counter.tv_sec * 1000000) + counter.tv_usec) / 1000000.f;
#endif /* !__NDS__ */
}

void
event_warp_mouse(uint16_t x, uint16_t y)
{
#if defined(__SDL__)
	warp = 1;
	SDL_WarpMouse(x, y);
#endif
}

