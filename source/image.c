#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*#if defined(__SDL__)
# include <SDL_image/SDL_image.h>
#else*/
# include <png.h>
//#endif

#include "ifopen.h"
#include "image.h"

void
img_flip_x(image *i) {
	/* TODO: Implement correctly!? */ 
	uint32_t tmp, *data;
	uint32_t x, y;
	
	data = i->data;
	for (y = 0; y < i->h; ++y) {
		for (x = 0; x < i->w/2; ++x) {
			tmp = data[(i->w - x - 1) + y * i->w];
			data[(i->w - x - 1) + y * i->w] = data[x + y * i->w];
			data[x + y * i->w] = tmp;
		}
	}
}

void
img_flip_y(image *i) { 
	uint32_t tmp, *data;
	uint32_t x, y;
	
	data = i->data;
	for (y = 0; y < i->h/2; ++y) {
		for (x = 0; x < i->w; ++x) {
			tmp = data[x + (i->h - y - 1) * i->w];
			data[x + (i->h - y - 1) * i->w] = data[x + y * i->w];
			data[x + y * (i->w)] = tmp;
		}
	}
}

void
img_free(image *i)
{
	if (i->filename)
		free(i->filename);

	if (i->data)
		free(i->data);
}

uint32_t
img_get_pixel(image *img, uint32_t x, uint32_t y)
{
	/* FIXME: Depends on type */ 
    return ((uint32_t*)img->data)[y * img->w + x];
}

image
img_load(const char *filename)
{
/*#if defined(__SDL__)
	SDL_Surfare *t = IMG_Load(file);
#else*/
	image img;
	FILE *fp;
	png_byte header[8];
	png_structp png_ptr;
	png_infop info_ptr, end_ptr;
	png_bytep *row_pointer;
	int 	i,
			is_png,
			rowbytes;
    png_uint_32 w, h;
	
	memset(&img, 0, sizeof(img));
	fp = ifopen(filename, "rb");
	if (!fp) {
		printf("img> couldn't open %s\n", filename);
		fclose(fp);
		return img;
	}

	fread(header, 1, 8, fp);
	is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png) {
		printf("img> not a png file: %s\n", filename);
		fclose(fp);
		return img;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		printf("img> failed to create reader.\n");
		fclose(fp);
		return img;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		printf("img> failed to create info.\n");
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(fp);
		return img;
	}

	end_ptr = png_create_info_struct(png_ptr);
	if (!end_ptr) {
		printf("img> failed to create end info.\n");
		png_destroy_read_struct(&png_ptr, (png_infopp) info_ptr, (png_infopp) NULL);
		fclose(fp);
		return img;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
		fclose(fp);
		return img; 
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &img.bpp, &img.type, NULL,
		NULL, NULL);
    img.w = w;
    img.h = h;
    if (PNG_COLOR_TYPE_RGB != img.type && PNG_COLOR_TYPE_RGB_ALPHA != img.type)
        printf("img> warning: color type should be 6 (rgb alpha) but is %d\n",
            img.type);
	
	/* Convert index color images to RGB images */
	if (img.type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb (png_ptr);
	
	/* Convert 1-2-4 bits grayscale images to 8 bits grayscale. */
	if (img.type == PNG_COLOR_TYPE_GRAY && img.bpp < 8)
		png_set_gray_to_rgb(png_ptr);
	
	if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha (png_ptr);
	
	if (img.bpp == 16)
		png_set_strip_16 (png_ptr);
	else if (img.bpp < 8)
		png_set_packing (png_ptr);
	
    png_read_update_info(png_ptr, info_ptr);
	rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	img.data = malloc(rowbytes * img.h * sizeof(png_byte));
	if (!img.data) {
		printf("img> failed to malloc img.data\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
		fclose(fp);
		return img;
	}

	row_pointer = malloc(img.h * sizeof(png_bytep));
	if (!row_pointer) {
		printf("img> failed to malloc row_pointer\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
		fclose(fp);
		free(img.data);
		img.data = 0;
		return img;
	}

	/* Set up pointers to row data */
	for (i = 0; i<img.h; ++i) {
		row_pointer[img.h - i - 1] = img.data + i * rowbytes;
	}

	png_read_image(png_ptr, row_pointer);
//#endif

	img.filename = strdup(filename);
	png_read_end (png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
	free(row_pointer);
	fclose(fp);
	return img;
}

void
img_set_pixel(image *img, uint32_t x, uint32_t y, uint32_t color)
{
	/* FIXME: Depends on type */ 
    ((uint32_t*)img->data)[y * img->w + x] = color;
}

