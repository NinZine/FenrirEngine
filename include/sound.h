#ifndef _SOUND_H_
#define _SOUND_H_

#include <stdbool.h>
#include <stdint.h>

/* TODO: This section is deprecated. */
#if !defined SWIGLUA

#if defined(__ANDROID__)
typedef int ALuint;
#elif defined(__APPLE__)
# include <OpenAL/al.h>
#else
# include <AL/al.h>
#endif
void s_free_buffer(ALuint n, ALuint *buf);
void s_free_source(ALuint *source, bool force);
void s_generate_source(ALuint *source);
void s_init();
void s_open_file(const char *filename, const char *type, ALuint *source);
void s_play(ALuint source);
void s_quit();
#endif

uint8_t s_ogg_open(const char *filename);
void    s_ogg_play(uint8_t id, bool force_restart);
bool    s_ogg_playing(uint8_t id);
bool    s_ogg_update(uint8_t id);
void    s_update();

#endif

