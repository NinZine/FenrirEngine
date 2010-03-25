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
	void	*next;
} blender_file_block;

typedef struct dna_struct_field {
	uint16_t type_idx;
	uint16_t name_idx;
} dna_struct_field;

typedef struct dna_struct {
	uint16_t type_idx;
	uint16_t fields;
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

static dna_struct* 			get_dna_struct(int32_t idx, const dna *d);
static dna_struct_field* 	get_field_by_index(int32_t idx, const dna_struct *ds,
								const dna *d);
static int32_t		get_field_index(const char *name, const dna_struct *ds,
						const dna *d);
static const char*	get_name_ptr(const char *characters, int n);
static void*		get_offset_by_index(int32_t index, const dna_struct *ds,
						const dna *d, char ptr_size);

static void 	model_free_block(blender_file_block *bfp, bool all_next);
static void 	model_read_file_block(const blender_header *bh,
            		blender_file_block *bfp, FILE *fp);

static dna		parse_dna(const blender_file_block *bfp);
static int32_t 	parse_int(const blender_header *bh, int32_t i);
static void		print_dna(const dna *d);
static void 	print_dna_struct(const dna_struct *dstr, const dna *d);

static void		read_scene(blender_file_block *bfp, const dna *d,
					const blender_header *bh);

dna_struct*
get_dna_struct(int32_t idx, const dna *d)
{
	dna_struct *tmp;
	char *ptr;
	int i;
	bool found;

	found = false;
	tmp = d->structure;
	ptr = (char*)tmp;
	for (i = 0; i < *d->structures; ++i) {
		if (i == idx) {
			break;
		}
		ptr += sizeof(dna_struct) + (tmp->fields * sizeof(dna_struct_field));
		tmp = (dna_struct*)ptr;
	}

	if (i == *d->structures) tmp = 0;

	return tmp;
}

dna_struct_field*
get_field_by_index(int32_t idx, const dna_struct *ds, const dna *d)
{
	dna_struct_field *f;

	f = (void*)ds + sizeof(dna_struct);
	f += idx;

	return f;
}

int32_t
get_field_index(const char *name, const dna_struct *ds, const dna *d)
{
	dna_struct_field *dsf;
	const char *tmp;
	int32_t i, j, len, tmp_len;
	char *tmp_ptr;

	/* Find name index by name */
	len = strlen(name);
	tmp = d->name;
	for (i = 0; i < *d->names; ++i) {
		tmp_len = len;

		/* Remove pointer */
		if ('*' == *tmp) ++tmp;
		/* Omit array and functions */
		if (0 != (tmp_ptr = strchr(tmp, '['))) tmp_len = tmp_ptr - tmp;
		else if (0 != (tmp_ptr = strchr(tmp, '('))) tmp_len = tmp_ptr - tmp;

		if (len == tmp_len && 0 == strncmp(name, tmp, tmp_len)) {
			break;
		}

		tmp = get_name_ptr(tmp, 1);
	}

	/* Find name index within the structure */
	if (i < *d->names) {
		dsf = (void*)ds + sizeof(dna_struct);
		for (j = 0; j < ds->fields; ++j) {
			if (i == dsf->name_idx) return j;
			dsf += 1;
		}
	}
	
	return -1; /* not found */
}

const char*
get_name_ptr(const char *characters, int n)
{
	const char *tmp;
	int i;

	tmp = characters;
	for (i = 0; i < n; ++i) {
		while ('\0' != *tmp) ++tmp;
		++tmp;
	}

	return tmp;
}

void*
get_offset_by_index(int32_t index, const dna_struct *ds, const dna *d,
	char ptr_size)
{
	dna_struct_field *f;
	uint32_t i, size, array_size;
	const char *name, *array_start, *array_end;
	void *tmp;

	f = (void*)ds + sizeof(dna_struct);
	tmp = 0;
	for (i = 0; i < index; ++i) {
		name = get_name_ptr(d->name, f->name_idx);
		
		/* Size of one element. Pointer, function pointer or data. */
		if ('*' == *name || '(' == *name) size = ptr_size;
		else size = d->byte[f->type_idx];
		
		/* Is it an array or even 2D array? */
		array_end = name;
		array_size = 1;
		while (0 != (array_start = strchr(array_end, '['))) {
			array_end = strchr(array_end, ']');
			array_size = array_size * atoi(array_start+1);
		}

		/* Add total offset for this field */
		tmp += size * array_size;
		f += 1;
	}

	return tmp;
}

void
model_free_block(blender_file_block *bfp, bool all_next)
{
	blender_file_block *next;
	
	do {
		next = bfp->next;
		free(bfp->data);
		free(bfp);
		bfp = next;
	} while (true == all_next && 0 != bfp);
}

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
    blender_file_block *bfp, *first_bfp;
	dna d;

    memset(&m, 0, sizeof(m));
    memset(&bh, 0, sizeof(bh));

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
	
	first_bfp = bfp = malloc(sizeof(blender_file_block));
    memset(bfp, 0, sizeof(bfp));
    model_read_file_block(&bh, bfp, fp);
    while (0 != strncmp(bfp->code, "ENDB", 4)) {
		if (0 == strncmp(bfp->code, "DNA1", 4)) {
			d = parse_dna(bfp);
			print_dna(&d);
		}
		bfp->next = malloc(sizeof(blender_file_block));
		bfp = bfp->next;
		memset(bfp, 0, sizeof(bfp));
        model_read_file_block(&bh, bfp, fp);
    }
	fclose(fp);

	read_scene(first_bfp, &d, &bh);

	model_free_block(first_bfp, true);
	return m;
}

void
model_read_file_block(const blender_header *bh, blender_file_block *bfp, FILE *fp)
{

    //printf("model> reading block\n");
    fread(bfp->code, 1, 4, fp);
    fread(&bfp->size, 4, 1, fp);
    fread(&bfp->old_adress, bh->ptr_size, 1, fp);
    fread(&bfp->sdna_idx, 4, 1, fp);
    fread(&bfp->count, 4, 1, fp);
    
	bfp->data = malloc(bfp->size);
	fread(bfp->data, 1, bfp->size, fp);
	//parse_int(bh, bfp->size);
    printf("model> blender code:%c%c%c%c dna:%d size:%d\n", bfp->code[0],
		bfp->code[1], bfp->code[2], bfp->code[3], bfp->sdna_idx, bfp->size);
}

/* Sets up then pointers in the dna structure. */
dna
parse_dna(const blender_file_block *bfp)
{
	dna 	d;
	char 	*tmp;
	int 	i;
	
	d.id = bfp->data;
	d.names = (int32_t*)bfp->data+2;	/* # names */

	tmp = bfp->data+12;					/* sequence of names start here */
	d.name = tmp;
	for (i = 0; i < *d.names; ++i) {
		while (0 != *tmp) ++tmp;
		++tmp;
	}

	tmp += 7;
	d.types = (int32_t*)tmp;			/* # types */
	tmp += 4;
	d.type = tmp;						/* sequence of type names start here */
	for (i = 0; i < *d.types; ++i) {
		while (0 != *tmp) ++tmp;
		++tmp;
	}

	tmp += 1;							/* TLEN */
	tmp += 4;

	d.byte = (int16_t*)tmp;				/* # bytes for each type */
	tmp += 2 * (*d.types);

	tmp += 4;							/* STRC */
	d.structures = (int32_t*)tmp;		/* # structures */
	tmp += 4;
	d.structure = (dna_struct*)tmp;		/* first structure */

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

void
print_dna(const dna *d)
{
	int i;
	char *tmp;
	
	printf("names: %d\n", *d->names);
	tmp = d->name;
	for (i = 0; i < *d->names; ++i) {
		printf("%s, ", tmp);
		
		while (0 != *tmp) ++tmp;
		++tmp;
	}

	printf("\ntypes: %d\n", *d->types);
	tmp = d->type;
	for (i = 0; i < *d->types; ++i) {
		printf("%s, ", tmp);
		
		while (0 != *tmp) ++tmp;
		++tmp;
	}

	tmp += 1;
	printf("\n%c%c%c%c\n", tmp[0], tmp[1], tmp[2], tmp[3]);
	for (i = 0; i < *d->types; ++i) {
		printf("%d, ", d->byte[i]);
	}

	tmp = (char*)&d->byte[i];
	printf("\n%c%c%c%c\n", tmp[0], tmp[1], tmp[2], tmp[3]);
	printf("structs: %d\n", *d->structures);
	tmp = (char*)d->structure;
	for (i = 0; i < *d->structures; ++i) {
		dna_struct *dstr;
		
		dstr = (dna_struct*)tmp;
		print_dna_struct(dstr, d);
		tmp += sizeof(dna_struct) + dstr->fields * sizeof(dna_struct_field);
	}
}

void
print_dna_struct(const dna_struct *dstr, const dna *d)
{
	dna_struct_field *f;
	int j;

	printf("structure:%s fields:%d size:%d\n", get_name_ptr(d->type, dstr->type_idx),
		dstr->fields, d->byte[dstr->type_idx]);
	f = (void*)dstr + sizeof(dna_struct);
	for (j = 0; j < dstr->fields; ++j) {
		printf("\tname: %s (%d) type: %s (%d)\n", get_name_ptr(d->name, f->name_idx),
			f->name_idx, get_name_ptr(d->type, f->type_idx), f->type_idx);
		f += 1; 
	}
}

void
read_scene(blender_file_block *first_bfp, const dna *d, const blender_header *bh)
{
	/* TODO: Goal, print id.name of scene struct */
	blender_file_block *bfp;
	dna_struct *scene_struct, *base;
	int32_t idx;
	bool found;
	void *offset;
	dna_struct_field *f;

	found = false;
	bfp = first_bfp;
	while (false == found && 0 != strncmp(bfp->code, "ENDB", 4)) {
		if (0 == strncmp(bfp->code, "SC", 2)) {
			found = true;
			break;
		}
		bfp = bfp->next;
	}

	scene_struct = get_dna_struct(bfp->sdna_idx, d);
	print_dna_struct(scene_struct, d);

	idx = get_field_index("id", scene_struct, d);
	idx = get_field_index("world", scene_struct, d);
	offset = get_offset_by_index(idx, scene_struct, d, bh->ptr_size);
	idx = get_field_index("frame_step", scene_struct, d);
	offset = get_offset_by_index(idx, scene_struct, d, bh->ptr_size);
	f = get_field_by_index(idx, scene_struct, d);
	
	idx = get_field_index("base", scene_struct, d);
	offset = get_offset_by_index(idx, scene_struct, d, bh->ptr_size);
	f = get_field_by_index(idx, scene_struct, d);
	base = get_dna_struct(f->type_idx, d);
	idx = get_field_index("first", base, d);
}

