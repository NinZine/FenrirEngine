#include <sys/time.h>

#if defined(__SDL__)
# include <SDL/SDL.h>
#elif defined(__NDS__)
# include <nds/nds.h>
# include <nds/timers.h>
#endif

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

