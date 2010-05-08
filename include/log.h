#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdio.h>

#if defined(__ANDROID__)
# include <android/log.h>
#endif /* !__ANDROID__ */

void log_print(const char *str);
void log_printf(const char *fmt, ...);

#endif

