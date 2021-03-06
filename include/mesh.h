#ifndef _MESH_H_
#define _MESH_H_

typedef struct mesh {
	char 		*filename;
	uint16_t	vertices;
	uint16_t 	faces;
    uint16_t    colors;

    char    *color;
	float 	*vertex;
	uint16_t *face;
} mesh;

#endif

