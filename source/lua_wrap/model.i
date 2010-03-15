%module model

%{
	#include "model.h"
%}

%rename (open_blender) model_open_blender;
%rename (open_cex) model_open_cex;

%include "../../include/model.h"

