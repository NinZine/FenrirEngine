#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#if defined(__ANDROID__)
# include <android/log.h>
#endif /* !__ANDROID__ */

#include "log.h"

static char **message;
static uint32_t messages = 0;
static uint32_t message_size = 0;

void log_flush()
{
}

const char *
log_get_last_line()
{
	uint32_t line;
	
	line = (messages > 0) ? messages - 1 : 0;
	return message[(messages > 0) ? messages - 1 : 0];
}

const char *
log_get_line(uint32_t line)
{
	if (line >= messages) line = messages-1;
	
	return message[line];
}

uint32_t
log_lines()
{
	return messages;
}

void log_print(const char *str)
{
	
	if (*str != '\n') {
		message_size += strlen(str) + 1;
		message = realloc(message, message_size);
		message[messages]= strdup(str);
#if defined(__ANDROID__)
		__android_log_write(3, "ConceptEngine", str);
#else
		printf("%s", message[messages]);
#endif
		
		++messages;
	} else {
		printf("\n");
	}
}

void log_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args,fmt);
#if defined(__ANDROID__)
    __android_log_vprint(3, "ConceptEngine", fmt, args);
#else
    vprintf(fmt,args);
#endif
    va_end(args);
}
