#ifndef _EVENT_H_
#define _EVENT_H_

#include <stdint.h>

enum event_t {
	EMPTY,
    KEYDOWN,
    KEYUP,
};

typedef union event {
    enum event_t type;

    struct {
        enum event_t type;
        char c;
        uint16_t sym;
    } key;
} event;

event   event_poll();
void    event_sleep(unsigned int ms);
float	event_time();

#endif

