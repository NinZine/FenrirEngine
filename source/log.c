#include "log.h"

void log_print(const char *str)
{
#if defined(__ANDROID__)
    __android_log_write(3, "ConceptEngine", str);
#else
    puts(str);
#endif
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
