%module event

%{
    #include "event.h"
%}

%rename(poll) event_poll;
%rename(sleep) event_sleep;
%rename(time) event_time;

%include <stdint.i>
%include "../../include/event.h"

