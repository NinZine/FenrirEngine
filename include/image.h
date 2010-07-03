#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdint.h>

typedef struct image {
	char 		*filename;
	uint32_t	w,
				h;
    int32_t		bpp;
    int32_t     type;
	void 		*data;
} image;

void 	    img_free(image *i);
uint32_t    img_get_pixel(image *img, uint32_t x, uint32_t y);
image 	    img_load(const char *filename);
void        img_set_pixel(image *img, uint32_t x, uint32_t y, uint32_t color);

#endif

