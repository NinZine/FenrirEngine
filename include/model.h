#ifndef _MODEL_H_
#define _MODEL_H_

#include <stdint.h>

typedef struct model {
	char 		*filename;
	uint16_t	vertices;
	uint32_t 	faces;

	float 	*vertex;
	uint16_t *face;
} model;

model model_open_cex(const char *filename);
model model_open_blender(const char *filename);

#endif

