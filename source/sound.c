#include <OpenAL/al.h>
#include <OpenAL/alc.h>
/*#include <OpenAL/oalStaticBufferExtension.h>
*/

#include "sound.h"

static void s_free_source_later(ALuint *source);

static ALCcontext	*context;
static ALCdevice	*device;
static ALuint		buffer[32];
static uint8_t		buffers = 0;
static ALuint		latched[255];

/*ALvoid  alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
	static	alBufferDataStaticProcPtr	proc = NULL;
    
    if (proc == NULL) {
        proc = (alBufferDataStaticProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");
    }
    
    if (proc)
        proc(bid, format, data, size, freq);
	
    return;
}*/

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
	//alSourcei(*source, AL_LOOPING, AL_TRUE);
}

void
s_init_openal()
{
	device = alcOpenDevice(0);
	
	if (!device) return;
	
	context = alcCreateContext(device, 0);
	alcMakeContextCurrent(context);
}

void
s_open_file(const char *filename, const char *type, ALuint *buf)
{
	
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
	
	
#if TARGET_OS_IPHONE
	OSStatus result = AudioFileOpenURL((CFURLRef)afUrl,
		kAudioFileReadPermission, 0, &outAFID);
#else
	OSStatus result = AudioFileOpenURL((CFURLRef)afUrl, fsRdPerm, 0, &outAFID);
#endif

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
	bzero(latched, sizeof(ALuint) * 255);
	
	for (i = 0; i < 32; ++i) {
		if (0 != buffer[i]) {
			alDeleteBuffers(1, &buffer[i]);
		}
	}
	bzero(buffer, sizeof(ALuint) * 32);
	buffers = 0;
	
	alcDestroyContext(context);
	alcCloseDevice(device);
	device = 0;
	context = 0;
}

void
s_update()
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

