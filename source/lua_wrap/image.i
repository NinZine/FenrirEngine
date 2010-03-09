%module image
%{
    #include "image.h"
%}

%rename(free) img_free;
%rename(load) img_load;

%include <stdint.i>
%include "../../include/image.h"

