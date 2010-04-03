#ifndef _MODEL_H_
#define _MODEL_H_

#include <stdint.h>

#include "mesh.h"

typedef struct blender_header {
    char id[7];
    char ptr_size;
    char endian;
    char version[3];
} blender_header;

typedef struct blender_file_block {
    char    code[4];
    int32_t size;
    void    *old_adress;
    int32_t sdna_idx;
    int32_t count;

    void    *data;
	struct blender_file_block *next;
} blender_file_block;

typedef struct dna_struct_field {
	uint16_t type_idx;
	uint16_t name_idx;
} dna_struct_field;

typedef struct dna_struct {
	uint16_t type_idx;
	uint16_t fields;

	/* TODO: Implement usage of?
	dna_struct_field *d;
	*/
} dna_struct;

typedef struct dna {
	char	*id;
	int32_t *names;
	int32_t *types;
	int32_t *structures;

	char 	*name;
	char	*type;
	int16_t *byte;
	dna_struct *structure;
} dna;

typedef struct blender_file {
	blender_header header;
	blender_file_block *block;
	dna d;
} blender_file;

void            blender_close(blender_file *bf);
mesh            blender_get_mesh(const char *name, blender_file *bf);
blender_file    blender_open(const char *filename);

#endif

