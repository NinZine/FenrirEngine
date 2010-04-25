%module image
%{
    #include "image.h"
%}

%rename(free) img_free;
%rename(get_pixel) img_get_pixel;
%rename(load) img_load;
%rename(set_pixel) img_set_pixel;

%include <stdint.i>
%include "../../include/image.h"

