#ifndef _SOUND_H_
#define _SOUND_H_

#if defined(__APPLE__)
# include <AudioToolbox/AudioToolbox.h>
# include <OpenAL/al.h>
#else
# include <AL/al.h>
#endif

void s_free_buffer(ALuint n, ALuint *buf);
void s_free_source(ALuint *source, bool force);
void s_generate_source(ALuint *source);
void s_init_openal();
void s_open_file(const char *filename, const char *type, ALuint *source);
void s_play(ALuint source);
void s_quit();
void s_update();

#endif

