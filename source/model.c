#include <sys/types.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "model.h"

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
} blender_file_block;

typedef struct dna_struct_field {
	uint16_t type_idx;
	uint16_t name_idx;
} dna_struct_field;

typedef struct dna_struct {
	uint16_t type_idx;
	uint16_t fields;

	dna_struct_field *field;
} dna_struct;

typedef struct dna {
	char	*id;
	int32_t *names;
	int32_t *types;
	int32_t *structures;

	char 	*name;
	char	*type;
	int16_t *bytes;
	dna_struct *structure;
} dna;

static void 	model_read_file_block(const blender_header *bh,
            		blender_file_block *bfp, FILE *fp);
static dna		parse_dna(const blender_file_block *bfp);
static int32_t 	parse_int(const blender_header *bh, int32_t i);

model
model_open_cex(const char *filename)
{
	FILE *fp;
	model m;

	memset(&m, 0, sizeof(m));
	fopen(filename, "rb");
	if (!fp) {
        printf("model> unable to open file %s\n", filename);
        fclose(fp);
        return m;
	}

	fread(&m.vertices, 4, 1, fp);
	fread(&m.faces, 4, 1, fp);

	return m;
}

model
model_open_blender(const char *filename)
{
    FILE *fp;
    model m;
    blender_header bh;
    blender_file_block bfp;

    memset(&m, 0, sizeof(m));
    memset(&bh, 0, sizeof(bh));
    memset(&bfp, 0, sizeof(bfp));

    fp = fopen(filename, "rb");
    if (!fp) {
        printf("model> unable to open file %s\n", filename);
        fclose(fp);
        return m;
    }

    fread(&bh, sizeof(bh), 1, fp);
    /* FIXME: bh.id contains BLENDER but strncmp fails */
    if (strncmp("BLENDER", bh.id, 7)) {
        printf("model> not a blender file %s\n", filename);
        fclose(fp);
        return m;
    }
    if ('_' == bh.ptr_size) bh.ptr_size = 4;
    else if ('-' == bh.ptr_size) bh.ptr_size = 8;
	
    model_read_file_block(&bh, &bfp, fp);
    while (0 != strncmp(bfp.code, "ENDB", 4)) {
		if (0 == strncmp(bfp.code, "DNA1", 4)) {
			parse_dna(&bfp);
		}
        memset(&bfp, 0, sizeof(bfp));
        model_read_file_block(&bh, &bfp, fp);
    }

	fclose(fp);
	return m;
}

void
model_read_file_block(const blender_header *bh, blender_file_block *bfp, FILE *fp)
{

    printf("model> reading block\n");
    fread(bfp->code, 1, 4, fp);
    fread(&bfp->size, 4, 1, fp);
    fread(&bfp->old_adress, bh->ptr_size, 1, fp);
    fread(&bfp->sdna_idx, 4, 1, fp);
    fread(&bfp->count, 4, 1, fp);
    
	bfp->data = malloc(bfp->size);
	fread(bfp->data, 1, bfp->size, fp);
	//parse_int(bh, bfp->size);
    printf("model> blender code: %c%c%c%c\n", bfp->code[0], bfp->code[1],
        bfp->code[2], bfp->code[3]);
}

dna
parse_dna(const blender_file_block *bfp)
{
	dna 	d;
	char 	*tmp;
	int 	i;
	
	d.id = bfp->data;
	d.names = (int32_t*)bfp->data+2;
	printf("names: %d\n", *d.names);

	tmp = bfp->data+12;
	for (i = 0; i < *d.names; ++i) {
		printf("%s, ", tmp);
		
		while (0 != *tmp) ++tmp;
		++tmp;
	}
	printf("\n");

	tmp += 7;
	d.types = (int32_t*)tmp;
	tmp += 4;
	printf("types: %d\n", *d.types);
	for (i = 0; i < *d.types; ++i) {
		printf("%s, ", tmp);
		
		while (0 != *tmp) ++tmp;
		++tmp;
	}
	printf("\n");

	tmp += 1;
	printf("%c%c%c%c\n", tmp[0], tmp[1], tmp[2], tmp[3]);
	tmp += 4;
	d.bytes = (int16_t*)tmp;
	for (i = 0; i < *d.types; ++i) {
		printf("%d, ", *(int16_t*)tmp);
		
		tmp += 2;
	}
	printf("\n");

	printf("%c%c%c%c\n", tmp[0], tmp[1], tmp[2], tmp[3]);
	tmp += 4;
	d.structures = (int32_t*)tmp;
	printf("structs: %d\n", *d.structures);
	tmp += 4;
	d.structure = (dna_struct*)tmp;
	for (i = 0; i < *d.structures; ++i) {
		dna_struct *dstr;
		int j;
		
		dstr = (dna_struct*)tmp;
		tmp += 4;
		printf("index: %d fields: %d\n", dstr->type_idx, dstr->fields);
		for (j = 0; j < dstr->fields; ++j) {
			dna_struct_field *f;

			f = (dna_struct_field*)tmp;
			tmp += 4;
			printf("\tindex: %d name: %d\n", f->type_idx, f->name_idx);
		}
	}

	return d;
}

int32_t
parse_int(const blender_header *bh, int32_t i)
{
	static bool big_endian = htons(1) == 1;
	int32_t j;

	printf("parse int: %8X ", i);
	if ('v' == bh->endian && big_endian) {
		j = ((i&0xff)<<24)+((i&0xff00)<<8)+((i&0xff0000)>>8)+((i>>24)&0xff);
		printf(" -> %8X\n", j);
	} else if ('V' == bh->endian && !big_endian) {
		j = (0xff & i) + (0xff00 & i << 8) + (0xff0000 & i << 16) +
		(0xff000000 & i << 24);
		printf(" -> %8X\n", j);
	}

	return j;
}

