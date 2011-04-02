#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <png.h>

#include "font.h"
#include "ifopen.h"
#include "rendering.h"

#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif
#ifndef min
	#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

typedef struct glyph {
	uint8_t ascii; // width;
	float x, y, width;
} glyph;


static const char letter[] =
	"abcdefghijklmnopqrstuvwxyz"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"1234567890~!@#$%^&*()-=+;:"
	"'\",./?[]|\\ <>`\xFF";

static font create_glyphs(FT_Face face, uint8_t *image, const uint8_t margin,
	uint16_t width, uint16_t height, uint16_t max_ascent, uint16_t max_descent);
static uint8_t* create_image(FT_Face face, const uint8_t margin,
	uint16_t width, uint16_t *height, uint16_t *ascent, uint16_t *descent);
static void flip_texture(uint8_t *image, uint16_t width, uint16_t height);

font
create_glyphs(FT_Face face, uint8_t *image, const uint8_t margin,
	uint16_t width, uint16_t height, uint16_t max_ascent, uint16_t max_descent)
{
	font f;
	uint16_t x = margin, y = margin + max_ascent;
	uint16_t i, row, pixel;
	glyph *entry;

	memset(&f, 0, sizeof(font));
	f.glyphs = (sizeof(letter)/sizeof(letter[0]));
	f.data = malloc(256 * sizeof(glyph));
	entry = f.data;

	// Drawing loop
	for (i = 0; i < f.glyphs; ++i) {
		uint16_t advance;
		uint16_t char_index = FT_Get_Char_Index(face,
			(unsigned int)letter[i]);
		float w, h;
		if (letter[i] == '\xFF') {
			char_index = 0;
		}

		// Render the glyph
		FT_Load_Glyph(face, char_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		// See whether the character fits on the current line
		advance = (face->glyph->metrics.horiAdvance >> 6) + margin;
		if (advance > width - x) {
			x = margin;
			y += (max_ascent + max_descent + margin);
		}

		// Fill in the GlyphEntry
		w = ((float)advance - margin) / width;
		h = ((float)y - max_ascent) / height;
		entry[(uint8_t)letter[i]].ascii = letter[i];
		entry[(uint8_t)letter[i]].width = w;
		entry[(uint8_t)letter[i]].x = (float)x / (float)width;
		entry[(uint8_t)letter[i]].y = h;

		// Copy the image gotten from FreeType onto the texture
		// at the correct position
		for (row = 0; row < face->glyph->bitmap.rows; ++row) {
			for (pixel = 0; pixel < face->glyph->bitmap.width; ++pixel) {
				image[(x + face->glyph->bitmap_left + pixel)
				+ (y - face->glyph->bitmap_top + row) * width]
				= face->glyph->bitmap.buffer[pixel + row
					* face->glyph->bitmap.pitch];
			}
		}
		x += advance; 
	}

	return f;
}

uint8_t*
create_image(FT_Face face, const uint8_t margin, uint16_t width,
	uint16_t *height, uint16_t *ascent, uint16_t *descent)
{
	uint16_t space_on_line = width - margin, lines = 1;
	uint16_t i = 0, needed_image_height = 0;
	uint8_t *image = 0;

	for (i = 0; i < sizeof(letter) / sizeof(letter[0]); ++i) {
		// Look up the character in the font file.
		uint16_t advance = 0;
		uint16_t char_index = FT_Get_Char_Index(face,
			(unsigned int)letter[i]);

		if (letter[i] == '\xFF')
		  char_index = 0;

		// Render the current glyph.
		FT_Load_Glyph(face, char_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		advance = (face->glyph->metrics.horiAdvance >> 6) + margin;
		// If the line is full go to the next line
		if (advance > space_on_line) {
		  space_on_line = width - margin;
		  ++lines;
		}
		space_on_line -= advance;

		*ascent = max(face->glyph->bitmap_top, *ascent);
		*descent = max(face->glyph->bitmap.rows -
		   face->glyph->bitmap_top, *descent);
	}
	// Compute how high the texture has to be.
	needed_image_height = ((*ascent) + (*descent) + margin)
		* lines + margin;
	// Get the first power of two in which it fits.
	*height = 16;
	while ((*height) < needed_image_height)
		(*height) *= 2;

	// Allocate memory for the texture, and set it to 0
	image = malloc((*height) * width * sizeof(uint8_t));
	memset(image, 0, (*height) * width * sizeof(uint8_t));

	return image;
}

/* TODO: Unused function, might come in handy some day. */
void
flip_texture(uint8_t *image, uint16_t width, uint16_t height) {
	uint8_t tmp;
	uint16_t x, y;

	for (y = 0; y < height/2; ++y) {
		for (x = 0; x < width; ++x) {
			tmp = image[x + (height-y) * width];
			image[x + (height-y) * width] = image[x + y * width];
			image[x + y * width] = tmp;
		}
	}
}

void
font_draw_string(font *f, const char *str, float x, float y)
{
	float *coord, *quad, *coords, *quads;
	uint16_t i, len, letters;
	glyph *g;

	len = strlen(str);
	coords = malloc(sizeof(float) * 8 * len);
	quads = malloc(sizeof(float) * 12 * len);

	r_push_matrix();
	r_bind_texture(f->texture);
	r_enable_blending();
	for (i = 0, letters = 0; i < len; ++i) {
		/* Line break */
		if ('\n' == str[i]) {
			y -= f->line_height * 64;
			x = 0;
			continue;
		}

		g = &((glyph*)f->data)[(uint8_t)str[i]];
		coord = &coords[letters * 8];
		quad = &quads[letters * 12];

		coord[0] = g->x; coord[1] = g->y + f->line_height;
		coord[2] = g->x + g->width; coord[3] = g->y + f->line_height;
		coord[4] = g->x; coord[5] = g->y;
		coord[6] = g->x + g->width; coord[7] = g->y;

		quad[0] = x; quad[1] = y; quad[2] = 0;
		quad[3] = x + g->width * 256; quad[4] = y; quad[5] = 0;
		quad[6] = x; quad[7] = y + f->line_height * 64; quad[8] = 0;
		quad[9] = x + g->width * 256; quad[10] = y + f->line_height * 64;
			quad[11] = 0;


		/* TODO: Fixme below */
		r_enable_texcoords(coord);
		r_render_vertices(quad, 4);
		
		x += g->width * 256;
		++letters;
	}
	/* FIXME: Renders wrong*/
	/*r_enable_texcoords(coords);
	r_render_vertices(quads, letters*4);*/
	r_disable_blending();
	r_pop_matrix();

	free(coords);
	free(quads);
}

void
font_free(font *i)
{
	if (i->filename)
		free(i->filename);

	if (i->data)
		free(i->data);
}

font
font_load(const char *filename, uint8_t size)
{
	FT_Face face;
	FT_Library library;
	FT_Open_Args args;
	font f;
	uint16_t len = 0;
	FILE *fp;
	uint8_t *buffer;
	uint16_t height = 0, max_ascent = 0, max_descent = 0;
	uint8_t *image;
	uint8_t margin = 3;
	
	memset(&f, 0, sizeof(font));
	if (FT_Init_FreeType(&library) != 0) {
		printf("font> initialize freetype2 failed\n");
		return f;
	}

	fp = ifopen(filename, "rb");
	if (!fp) {
		printf("font> couldn't open %s\n", filename);
		fclose(fp);
		return f;
	}
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buffer = malloc(len * sizeof(uint8_t));
	fread(buffer, 1, len, fp);
	fclose(fp);

    args.flags = FT_OPEN_MEMORY;
    args.memory_base = (const FT_Byte*) buffer;
    args.memory_size = len;
    if (FT_Open_Face(library, &args, 0, &face) != 0) {
		printf("font> couldn't create face object for font %s\n",
			filename);
		free(buffer);
		return f;
	}
	if (!(face->face_flags & FT_FACE_FLAG_SCALABLE)
	|| !(face->face_flags & FT_FACE_FLAG_HORIZONTAL)) {
		printf("font> %s is not scalable, aborting\n",
			filename);
		free(buffer);
		return f;
	}

	FT_Set_Pixel_Sizes(face, size, 0);
	image = create_image(face, margin, 256, &height, &max_ascent, &max_descent);
	f = create_glyphs(face, image, margin, 256, height, max_ascent,
		max_descent);
	f.filename = strdup(filename);
	f.texture = r_upload_texture(256, height, 8, PNG_COLOR_MASK_ALPHA, image);
	f.line_height = (float)(max_ascent + max_descent + margin) / (float)height;

	free(image);
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	free(buffer);

	return f;
}

