/*
 *	THIS FILE IS JUST SO ANDROID COMPILES WITHOUT ALTERING THE ACTUAL SOUND.C
 *	REMOVE WHEN SOUND HAS BEEN PORTED TO ANDROID
 */

#include "sound.h"

typedef struct ogg_file {
    int x;
} ogg_file;

void
s_free_buffer(ALuint n, ALuint *buf)
{
}

void
s_free_source(ALuint *source, bool force)
{
}

void
s_free_source_later(ALuint *source)
{
}

void
s_generate_source(ALuint *source)
{
}

void
s_init()
{
}

uint8_t
s_ogg_open(const char *filename)
{
    return 0;
}

void
s_ogg_play(uint8_t id, bool force_restart)
{
}

bool
s_ogg_playing(uint8_t id)
{
	return false;
}

bool
s_ogg_update(uint8_t id)
{
	return false;
}

void
s_update()
{
}

void
s_open_file(const char *filename, const char *type, ALuint *buf)
{	
}

void
s_play(ALuint source)
{
}

void
s_play_music(const char *filename, const char *type)
{		
}

void
s_quit()
{}

void
s_update_old()
{
}

