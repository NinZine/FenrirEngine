#ifndef _FONT_H_
#define _FONT_H_

#include <stdint.h>

typedef struct font {
	char 		*filename;
    float		line_height;
	uint16_t	texture,
				tex_height,
				tex_width;
	uint32_t	glyphs;
	void 		*data;
} font;

void		font_draw_string(font *f, const char *str, float x, float y);
void 	    font_free(font *i);
font 		font_load(const char *filename, uint8_t size);

#endif

