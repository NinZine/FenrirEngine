#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>

const char* log_get_last_line();
const char* log_get_line(uint32_t line);
uint32_t	log_lines();
void		log_print(const char *str);
void		log_printf(const char *fmt, ...);

#endif

