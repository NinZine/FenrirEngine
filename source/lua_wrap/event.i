%module event

%{
    #define SWIG_init_user SWIG_init_user_event
    #include "event.h"
%}

%rename(poll) event_poll;
%rename(sleep) event_sleep;

%include "../../include/event.h"

