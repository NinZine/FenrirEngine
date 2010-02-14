%module vec3

%{
    #include "vec3.h"
%}

%rename(add) vec3_add;
%rename(cross) vec3_cross;
%rename(dot) vec3_dot;
%rename(length) vec3_length;
%rename(lerp) vec3_lerp;
%rename(mul) vec3_mul;
%rename(normalize) vec3_normalize;
%rename(project) vec3_project;
%rename(slerp) vec3_slerp;
%rename(sub) vec3_sub;

%include "../../include/vec3.h"

