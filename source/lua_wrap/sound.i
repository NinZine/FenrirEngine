%module sound 

%{
    #include "sound.h"
%}

%rename(ogg_open) s_ogg_open;
%rename(ogg_play) s_ogg_play;
%rename(ogg_playing) s_ogg_playing;
%rename(ogg_update) s_ogg_update;
%rename(update) s_update;

%include <stdint.i>
%include "../../include/sound.h"

