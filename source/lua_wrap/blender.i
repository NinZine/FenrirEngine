%module blender

%{
	#include "blender.h"
%}

%rename (close) blender_close;
%rename (get_mesh) blender_get_mesh;
%rename (open) blender_open;

%include "../../include/blender.h"

