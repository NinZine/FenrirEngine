%module sys 

%{
    #include "system.h"
%}

%rename(exec) sys_exec_string;

%include "../../include/system.h"
