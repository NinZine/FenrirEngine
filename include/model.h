#ifndef _MODEL_H_
#define _MODEL_H_

typedef struct model {
	char 	*filename;
	int32_t	vertices;
	int32_t	faces;

	float 	*vertex;
	int16_t *face;
} model;

model model_open_cex(const char *filename);
model model_open_blender(const char *filename);

#endif

