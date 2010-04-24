#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(__APPLE__)
# include <OpenAL/al.h>
# include <OpenAL/alc.h>
# include <OpenAL/oalStaticBufferExtension.h>
#else
# include <AL/al.h>
# include <AL/alc.h>
# include <AL/alext.h>
#endif

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

#include "sound.h"

typedef struct ogg_file {
    uint8_t id;
    FILE *file_handle;
    OggVorbis_File file;
    vorbis_info *info;
    
    ALuint buffer[2];
    ALuint source;
    ALenum format;

    bool stream;
} ogg_file;

static void s_free_source_later(ALuint *source);

static ALCcontext	*context;
static ALCdevice	*device;
static ALuint		buffer[32];
static uint8_t		buffers = 0;
static ALuint		latched[255];

static const uint32_t BUFFER_SIZE = 4096;
static uint8_t ogg_files = 0;
static uint8_t ogg_ids = 0;
static ogg_file *oggs = 0;

ALvoid
alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid* data, ALsizei size,
    ALsizei freq)
{
#if defined __APPLE__
	static alBufferDataStaticProcPtr proc = NULL;

    if (proc == NULL) {
        proc = (alBufferDataStaticProcPtr) alcGetProcAddress(NULL,
            (const ALCchar*) "alBufferDataStatic");
    }
#else
	static PFNALBUFFERDATASTATICPROC proc = NULL;

    if (proc == NULL) {
        proc = (PFNALBUFFERDATASTATICPROC) alcGetProcAddress(NULL,
            (const ALCchar*) "alBufferDataStatic");
    }
#endif
    
    if (proc)
        proc(bid, format, data, size, freq);
	
    return;
}

static bool 
check_al_error()
{
    ALenum e;

    e = alGetError();
    if (e != AL_NO_ERROR) {
        printf("sound> OpenAL error: %s\n", alGetString(e));
    }
    return (e != AL_NO_ERROR);
}

static const char*
ogg_error(int code)
{
    switch (code) {
        default:
            return "unknown error";
    }
}

static void
ogg_print_comment(const vorbis_comment *vc)
{
    int i;

    printf("vendor: %s\n", vc->vendor);

    for (i=0; i<vc->comments; ++i) {
        printf("\t%s\n", vc->user_comments[i]);
    }
}

static void
ogg_print_info(const vorbis_info *vi)
{

    printf("version: %d\nchannels: %d\nrate (hz): %ld\nbitrate (upper): %ld\n"
            "bitrate (nominal): %ld\nbitrate (lower):%ld\nbitrate_window: %ld\n",
        vi->version, vi->channels, vi->rate, vi->bitrate_upper, vi->bitrate_nominal,
        vi->bitrate_lower, vi->bitrate_window);
}

static bool
ogg_stream(ogg_file *of, ALuint buffer)
{
    char data[BUFFER_SIZE];
    int size = 0;
    int section;
    int result;

    while (size < BUFFER_SIZE) {
        result = ov_read(&of->file, data + size, BUFFER_SIZE - size, 0, 2, 1, &section);
        if (result > 0) {
            size += result;
        } else if (result < 0) {
            printf("sound> error streaming ogg\n");
            return false;
        } else {
            break;
        }
    }

    if (0 == size) return false;

    alBufferData(buffer, of->format, data, size, of->info->rate);
    assert(check_al_error() == false);
    return true;
}

void
s_free_buffer(ALuint n, ALuint *buf)
{
	
	alDeleteBuffers(n, buf);
	
	/* TODO: Find buffers and zero it out */
}

void
s_free_source(ALuint *source, bool force)
{
	
	if (true == force) {
		alDeleteSources(1, source);
	} else {
		ALint tmp;
		
		alGetSourcei(*source, AL_SOURCE_STATE, &tmp);
		if (AL_PLAYING != tmp) {
			alDeleteSources(1, source);
		} else {
			s_free_source_later(source);
		}
	}
}

void
s_free_source_later(ALuint *source)
{
	uint8_t i;
	bool found = false;
	
	for (i = 0; i < 255 && false == found; ++i) {
		if (0 == latched[i]) {
			latched[i] = *source;
			found = true;
		}
	}
	
	/* Free anyway */
	if (false == found) {
		alDeleteSources(1, source);
	}
}

void
s_generate_source(ALuint *source)
{
	
	alGenSources(1, source);
	alSourcef(*source, AL_PITCH, 1.0f);
	alSourcef(*source, AL_GAIN, 1.0f);
	alSourcef(*source, AL_ROLLOFF_FACTOR, 0.0f);
	alSourcei(*source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(*source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(*source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
    alSource3f(*source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	//alSourcei(*source, AL_LOOPING, AL_TRUE);
}

void
s_init()
{
    static const ALfloat ori[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
	device = alcOpenDevice(NULL);
	
	if (!device) {
        printf("sound> failed to open audio device\n");
        return;
    }
	
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
    if (!context) {
        printf("sound> failed to create context\n");
    }

    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, ori);
    check_al_error();
}

uint8_t
s_ogg_open(const char *filename)
{
    OggVorbis_File stream;
    vorbis_info *vi;
    vorbis_comment *vc;
    FILE *f;
    int result;

    f = fopen(filename, "rb");
    if (NULL == f) {
        printf("sound> failed to open file %s\n", filename);
        return 0;
    }

    result = ov_open(f, &stream, NULL, 0);
    if (result < 0) {
        fclose(f);
        printf("sound> failed to read ogg stream. %s\n", ogg_error(result));
        return 0;
    }

    vi = ov_info(&stream, -1);
    vc = ov_comment(&stream, -1);
    ogg_print_info(vi);
    ogg_print_comment(vc);

    if (0 == oggs) {
        oggs = malloc(sizeof(ogg_file));
        ogg_files += 1;
    } else {
        ogg_files += 1;
        oggs = realloc(oggs, ogg_files * sizeof(ogg_file));
    }

    oggs[ogg_files-1].id = ++ogg_ids;
    oggs[ogg_files-1].file_handle = f;
    oggs[ogg_files-1].file = stream;
    oggs[ogg_files-1].info = vi;
    oggs[ogg_files-1].stream = true;
    
    oggs[ogg_files-1].format =
        (1 == vi->channels) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    
    alGenBuffers(2, oggs[ogg_files-1].buffer);
    s_generate_source(&oggs[ogg_files-1].source);

    check_al_error();
    return ogg_ids;
}

void
s_ogg_play(uint8_t id, bool force_restart)
{
    ogg_file *f;
    ALenum state;

    f = &oggs[id-1];
    alGetSourcei(f->source, AL_SOURCE_STATE, &state);
    check_al_error();

    if (AL_PLAYING == state && !force_restart) {
        return;
    }

    if (force_restart) {
        alSourceStop(f->source);
        ov_raw_seek(&f->file, 0);
        alSourceUnqueueBuffers(f->source, 2, f->buffer);
        assert(check_al_error() == false);
    }

    if (!ogg_stream(f, f->buffer[0]))
        return;
    if (!ogg_stream(f, f->buffer[1]))
        return;

    alSourceQueueBuffers(f->source, 2, f->buffer);
    check_al_error();
    alSourcePlay(f->source);
    check_al_error();
}

bool
s_ogg_playing(uint8_t id)
{
    ALint value;

    alGetSourcei(oggs[id-1].source, AL_SOURCE_STATE, &value);
    return (AL_PLAYING == value);
}

bool
s_ogg_update(uint8_t id)
{
    ogg_file *f;
    int value;
    bool active;

    /* FIXME: Search for id. */
    f = &oggs[id-1];
    active = false;

    alGetSourcei(f->source, AL_BUFFERS_PROCESSED, &value);
    check_al_error();
    while(value--) {
        ALuint buffer;

        alSourceUnqueueBuffers(f->source, 1, &buffer);
        check_al_error();
        active = ogg_stream(f, buffer);
        alSourceQueueBuffers(f->source, 1, &buffer);
        check_al_error();
    }

    if (!active)
        return active;

    alGetSourcei(f->source, AL_SOURCE_STATE, &value);
    if (AL_PLAYING != value) {
        printf("sound> warning: restarting sound, possible buffer underrun\n");
        alSourcePlay(f->source);
    }

    return active;
}

void
s_update()
{
    int i;

    for (i=0; i<ogg_files; ++i) {
        if (true == oggs[i].stream) {
            s_ogg_update(oggs[i].id);
        }
    }
}

void
s_open_file(const char *filename, const char *type, ALuint *buf)
{
#if defined(__APPLE__)	
	AudioFileID outAFID;
	CFStringRef f = CFStringCreateWithCString(NULL, filename,
		kCFStringEncodingISOLatin1);
	CFStringRef t = CFStringCreateWithCString(NULL, type,
		kCFStringEncodingISOLatin1);
	CFBundleRef main_bundle = CFBundleGetMainBundle();
	CFURLRef afUrl = CFBundleCopyResourceURL(main_bundle, f,
		t, CFSTR(""));
	UInt64 outDataSize = 0;
	UInt32 thePropSize = sizeof(UInt64);
	unsigned char *data;
	
	
# if TARGET_OS_IPHONE
	OSStatus result = AudioFileOpenURL((CFURLRef)afUrl,
		kAudioFileReadPermission, 0, &outAFID);
# else
	OSStatus result = AudioFileOpenURL((CFURLRef)afUrl,
	    fsRdPerm, 0, &outAFID);
# endif

	if (result != 0) printf("Cannot open file: %x", afUrl);	
	result = AudioFileGetProperty(outAFID,
		kAudioFilePropertyAudioDataByteCount, &thePropSize, &outDataSize);
	if (0 != result) printf("Cannot determine file size!");
	
	data = malloc((UInt32)outDataSize);
	result = AudioFileReadBytes(outAFID, false, 0, (UInt32*)&outDataSize, data);
	
	if (0 != result) printf("Cannot copy data!");
	AudioFileClose(outAFID);
	
	/* Put sound data into a new buffer */
	assert(buffers < 32);
	
	/* TODO: FIND an empty buffer spot */
	alGenBuffers(1, &buffer[buffers]);
	alBufferData(buffer[buffers], AL_FORMAT_STEREO16, data, outDataSize,
		44100);
	*buf = buffer[buffers];
	buffers += 1;
	
	free(data);
	CFRelease(afUrl);
	CFRelease(t);
	CFRelease(f);
#endif
}

void
s_play(ALuint source)
{
	
	/* TODO: Track when the buffers are used */
	alSourcePlay(source);
}

void
s_play_music(const char *filename, const char *type)
{
	
	
}

void
s_quit()
{
	uint8_t i;
	
	/* Free latched sources */
	for (i = 0; i < 255; ++i) {
		if (0 != latched[i]) {
			alDeleteSources(1, &latched[i]);
		}
	}
	memset(latched, 0, sizeof(ALuint) * 255);
	
	for (i = 0; i < 32; ++i) {
		if (0 != buffer[i]) {
			alDeleteBuffers(1, &buffer[i]);
		}
	}
	memset(buffer, 0, sizeof(ALuint) * 32);
	buffers = 0;
	
    alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
	device = 0;
	context = 0;
}

void
s_update_old()
{
	ALint state;
	uint8_t i;
	
	for (i = 0; i < 255; ++i) {
		if (0 != latched[i]) {
			alGetSourcei(latched[i], AL_SOURCE_STATE, &state);
			if (AL_PLAYING != state) {
				alDeleteSources(1, &latched[i]);
				latched[i] = 0;
			}
		}
	}
}

