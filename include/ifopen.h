/*-
 * License
 */

#ifndef _IFOPEN_H_
#define _IFOPEN_H_

#include <stdio.h>

#if defined(__IPHONE__)
const char* filename_to_path(char *filename);
const char* full_path();
char* full_path_to_file(const char *filename);
FILE *ifopen(const char *name, const char *mode);
#else
# define ifopen(n,m) fopen(n, m)
#endif

#endif