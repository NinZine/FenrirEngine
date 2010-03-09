#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdint.h>

typedef struct image {
	char 		*filename;
	uint32_t	w,
				h;
	int			bpp;
	void 		*data;
} image;

void 	img_free(image *i);
image 	img_load(const char *filename);

#endif

