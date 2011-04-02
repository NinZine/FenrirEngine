%module font 
%{
    #include "font.h"
%}

%rename(draw_string) font_draw_string;
%rename(free) font_free;
%rename(load) font_load;

%include <stdint.i>
%include "../../include/font.h"

