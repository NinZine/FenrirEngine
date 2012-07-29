%module event

%{
    #include "event.h"
%}

%rename(poll) event_poll;
%rename(show_cursor) event_show_cursor;
%rename(sleep) event_sleep;
%rename(time) event_time;
%rename(warp_mouse) event_warp_mouse;

%include <stdint.i>
%include "../../include/event.h"

