%module md2 

%{
	#include "md2.h"
%}

%rename (cleanup_render) md2_cleanup_render;
%rename (free) md2_free;
%rename (open) md2_open;
%rename (print) md2_print;
%rename (render) md2_render;
%rename (set_animation) md2_set_animation;
%rename (setup_render) md2_setup_render;
%rename (tick) md2_tick;

%include <stdint.i>
%include "../../include/md2.h"

