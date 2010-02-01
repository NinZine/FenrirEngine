/*
 *  sound.h
 *  HooverCat
 *
 *  Created by Andreas Kröhnke on 8/8/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#ifndef _SOUND_H_
#define _SOUND_H_

#include <AudioToolbox/AudioToolbox.h>
#include <OpenAL/al.h>

void s_free_buffer(ALuint n, ALuint *buf);
void s_free_source(ALuint *source, bool force);
void s_generate_source(ALuint *source);
void s_init_openal();
void s_open_file(const char *filename, const char *type, ALuint *source);
void s_play(ALuint source);
void s_quit();
void s_update();

#endif
