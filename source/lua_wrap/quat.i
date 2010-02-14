%module quat
%import(module="mat4") "mat4.i"

%{
    #include "quat.h"
%}
%rename(add) quat_add;
%rename(dot) quat_dot;
%rename(from_axis) quat_from_axis;
%rename(length) quat_length;
%rename(lerp) quat_lerp;
%rename(mul) quat_mul;
%rename(normalize) quat_normalize;
%rename(scale) quat_scale;
%rename(slerp) quat_slerp;
%rename(sub) quat_sub;
%rename(to_axis) quat_to_axis;
%rename(to_mat4) quat_to_mat4;

%include "../../include/quat.h"

