#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*#if defined(__SDL__)
# include <SDL_image/SDL_image.h>
#else*/
# include <png.h>
//#endif

#include "image.h"

void
img_free(image *i)
{
	if (i->filename)
		free(i->filename);

	if (i->data)
		free(i->data);
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
	int 	color_type,
			i,
			is_png,
			rowbytes;
	
	memset(&img, 0, sizeof(img));
	fp = fopen(filename, "rb");
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
	png_get_IHDR(png_ptr, info_ptr, &img.w, &img.h, &img.bpp, &color_type, NULL,
		NULL, NULL);
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
		free(&img.data);
		img.data = 0;
		return img;
	}

	for (i = 0; i<img.h; ++i) {
		row_pointer[img.h - i - 1] = img.data + i * rowbytes;
	}

	png_read_image(png_ptr, row_pointer);
//#endif

	img.filename = strdup(filename);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
	free(row_pointer);
	fclose(fp);
	return img;
}

