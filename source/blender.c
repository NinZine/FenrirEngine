#include <sys/types.h>

#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "blender.h"
#include "vec3.h"

static void convert_quad_to_tri(int32_t idx, uint16_t *new_face,
                const int32_t *old_face, float *vertex);

static blender_file_block*	dereference_ptr(blender_file_block *first_bfp,
								void *ptr, char ptr_size);

static void free_file_block(blender_file_block *bfp, bool all_next);

static uint32_t
get_address(const char *name, blender_file_block *bfp, blender_file *bf,
    dna_struct **ds_out);
static dna_struct* 			get_dna_struct(int32_t idx, const dna *d);
static dna_struct* 			get_dna_struct_by_type_index(int32_t idx,
								const dna *d);
static dna_struct_field* 	get_field_by_index(int32_t idx,
								const dna_struct *ds, const dna *d);
static int32_t		get_field_index(const char *name, const dna_struct *ds,
						const dna *d);

static const char*	get_name_ptr(const char *characters, int n);
static blender_file_block* get_object(const char *name, int16_t type,
                        blender_file *bf);
static uint32_t		get_offset_by_index(int32_t index, const dna_struct *ds,
						const dna *d, char ptr_size);
static uint32_t		get_offset_by_name(const char *name, const dna_struct *ds,
						const dna *d, char ptr_size);
static blender_file_block* get_scene_block(blender_file_block *first_block);

static dna		parse_dna(const blender_file_block *bfp);
static int32_t 	parse_int(const blender_header *bh, int32_t i);
static void		print_dna(const dna *d);
static void 	print_dna_struct(const dna_struct *dstr, const dna *d);

static void 	read_file_block(const blender_header *bh,
            		blender_file_block *bfp, FILE *fp);

void
blender_close(blender_file *bf)
{

    if (0 != bf->block) free_file_block(bf->block, true);
}

mesh
blender_get_mesh(const char *name, blender_file *bf)
{
    blender_file_block *bfb;
    dna_struct *tmp_struct;
    mesh m;
    float *v;
    uint32_t address, offset;
    int32_t i, j, *face;

    memset(&m, 0, sizeof(m));
    bfb = get_object(name, 1, bf);
    if (0 == bfb) return m;

    address = get_address("data.totvert", bfb, bf, NULL);
    if (0 == address)
        printf("blender> data.totvert adress not found in mesh.\n");
    m.vertices = *(int32_t*)address;
    address = get_address("data.totface", bfb, bf, NULL);
    m.faces = *(int32_t*)address;

    /* Read vertices */
	address = get_address("data.mvert", bfb, bf, &tmp_struct);
	offset = 0;
	m.vertex = malloc(m.vertices * 3 * sizeof(float));
	for (i = 0; i < m.vertices; ++i) {
		v = (float*)(address+offset);
		((vec3*)m.vertex)[i].x = v[0];
		((vec3*)m.vertex)[i].y = v[2];
		((vec3*)m.vertex)[i].z = v[1];
		printf("(%.2f, %.2f, %.2f) ", m.vertex[i*3], m.vertex[i*3+1],
		m.vertex[i*3+2]); 
		offset += bf->d.byte[tmp_struct->type_idx];
	}

    /* Read faces */
    address = get_address("data.mface", bfb, bf, &tmp_struct);
	offset = get_offset_by_name("v1", tmp_struct, &bf->d, bf->header.ptr_size);
	m.face = malloc(m.faces * 2 * 3 * sizeof(uint16_t)); /* 2 x Triangle x 3 verts */
	for (i = 0, j = 0; i < m.faces; ++i) {
		face = (int32_t*)(address+offset);

		if (face[3] > 0) {
                convert_quad_to_tri(j, m.face, face, m.vertex);
				j += 6;
		} else {
			m.face[j] = face[0];
			m.face[j+1] = face[1];
			m.face[j+2] = face[2];
			j += 3;
		}
		offset += bf->d.byte[tmp_struct->type_idx];
	}

	m.faces = m.faces * 2; /* Quads = 2 Triangles */
	j = j/3;
	if (j != m.faces) {
		m.faces = j;
		face = (int32_t*)m.face;
		j = j * 3 * sizeof(uint16_t);
		m.face = malloc(j);
		memcpy(m.face, face, j);
		free(face);
	}

	printf("\ntriangles: %d\n", m.faces);

    return m;
}

blender_file
blender_open(const char *filename)
{
    FILE *fp;
    mesh m;
    blender_file bf;
    blender_file_block *bfp;

    memset(&m, 0, sizeof(m));
    memset(&bf, 0, sizeof(bf));

    fp = fopen(filename, "rb");
    if (!fp) {
        printf("model> unable to open file %s\n", filename);
        fclose(fp);
        return bf;
    }

    fread(&bf.header, sizeof(bf.header), 1, fp);
    /* FIXME: bh.id contains BLENDER but strncmp fails */
    if (strncmp("BLENDER", bf.header.id, 7)) {
        printf("model> not a blender file %s\n", filename);
        fclose(fp);
        return bf;
    }
    if ('_' == bf.header.ptr_size) bf.header.ptr_size = 4;
    else if ('-' == bf.header.ptr_size) bf.header.ptr_size = 8;
	
	bf.block = bfp = malloc(sizeof(blender_file_block));
    memset(bfp, 0, sizeof(blender_file_block));
    read_file_block(&bf.header, bfp, fp);
    while (0 != strncmp(bfp->code, "ENDB", 4)) {
		if (0 == strncmp(bfp->code, "DNA1", 4)) {
			bf.d = parse_dna(bfp);
			print_dna(&bf.d);
		}
		bfp->next = malloc(sizeof(blender_file_block));
		bfp = bfp->next;
		memset(bfp, 0, sizeof(blender_file_block));
        read_file_block(&bf.header, bfp, fp);
    }
	fclose(fp);

	return bf;
}

void
convert_quad_to_tri(int32_t idx, uint16_t *new_face, const int32_t *old_face,
    float *vertex)
{
    vec3 *v1, *v2, *v3, *v4, u1, u2;

    v1 = ((vec3*)vertex)+old_face[0];
    v2 = ((vec3*)vertex)+old_face[1];
    v3 = ((vec3*)vertex)+old_face[2];
    v4 = ((vec3*)vertex)+old_face[3];

    u1 = vec3_sub(v1, v3);
    u2 = vec3_sub(v2, v4);

    if (vec3_length(&u1) - vec3_length(&u2) > 0.0f) {
        new_face[idx] = old_face[0];
        new_face[idx+1] = old_face[1];
        new_face[idx+2] = old_face[2];
        new_face[idx+3] = old_face[0];
        new_face[idx+4] = old_face[2];
        new_face[idx+5] = old_face[3];
    } else {
        new_face[idx] = old_face[0];
        new_face[idx+1] = old_face[1];
        new_face[idx+2] = old_face[3];
        new_face[idx+3] = old_face[1];
        new_face[idx+4] = old_face[2];
        new_face[idx+5] = old_face[3];
    }
}

blender_file_block*
dereference_ptr(blender_file_block *first_bfp, void *ptr, char ptr_size)
{
	blender_file_block *bfp, *walker;
	uint64_t ptr_val;

	if (4 == ptr_size) ptr_val = *(uint32_t*)ptr;
	else ptr_val = *(uint64_t*)ptr;

	bfp = 0;
	walker = first_bfp;
	while (0 == bfp && 0 != strncmp(walker->code, "ENDB", 4)) {
		if (4 == ptr_size)  {
			if ((uint32_t)walker->old_adress == ptr_val)
				bfp = walker;
		} else {
			/* FIXME: Warning here about size of pointer. */
			if ((uint64_t)walker->old_adress == ptr_val)
				bfp = walker;
		}

		walker = walker->next;
	}

	return bfp;
}

void
free_file_block(blender_file_block *bfp, bool all_next)
{
	blender_file_block *next;
	
	do {
		next = bfp->next;
		free(bfp->data);
		free(bfp);
		bfp = next;
	} while (true == all_next && 0 != bfp);
}

uint32_t
get_address(const char *name, blender_file_block *bfp, blender_file *bf,
    dna_struct **ds_out)
{
    dna_struct *ds;
	dna_struct_field *f;
	char *buffer, *word, *reentry;
    const char *tmp_name;
	char *sep = ".";
	int32_t idx;
	uint32_t offset;

	buffer = strdup(name);
	offset = 0;
    ds = get_dna_struct(bfp->sdna_idx, &bf->d);
	for (word = strtok_r(buffer, sep, &reentry); word;
		 word = strtok_r(NULL, sep, &reentry)) {
		
		idx = get_field_index(word, ds, &bf->d);
		offset += get_offset_by_index(idx, ds, &bf->d, bf->header.ptr_size);
		f = get_field_by_index(idx, ds, &bf->d);
        tmp_name = get_name_ptr(bf->d.name, f->name_idx);
        /* Follow pointer */
        if (*tmp_name == '*') {
            bfp = dereference_ptr(bf->block, bfp->data+offset, bf->header.ptr_size);
            ds = get_dna_struct(bfp->sdna_idx, &bf->d);
            offset = 0;
        } else {
		    ds = get_dna_struct_by_type_index(f->type_idx, &bf->d);
		}

        if (0 != ds)
			print_dna_struct(ds, &bf->d);
	}

	free(buffer);
    if (NULL != ds_out)
        *ds_out = ds;
	return (uint32_t)(bfp->data+offset);
}

dna_struct*
get_dna_struct(int32_t idx, const dna *d)
{
	dna_struct *tmp;
	char *ptr;
	int i;

	tmp = d->structure;
	ptr = (char*)tmp;
	/* Structure -> n field declarations -> Next structure */
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

dna_struct*
get_dna_struct_by_type_index(int32_t idx, const dna *d)
{
	dna_struct *tmp;
	char *ptr;
	int i;

	tmp = d->structure;
	ptr = (char*)tmp;
	for (i = 0; i < *d->structures; ++i) {
		if (tmp->type_idx == idx) {
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

	/* FIXME: There are names with the same name, just different size/type. */
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
			/* Find name index within the structure */
			dsf = (void*)ds + sizeof(dna_struct);
			for (j = 0; j < ds->fields; ++j) {
				if (i == dsf->name_idx) return j;
				dsf += 1;
			}
		}

		tmp = get_name_ptr(tmp, 1);
	}

	return -1; /* not found */
}

const char*
get_name_ptr(const char *characters, int n)
{
	const char *tmp;
	int i;

	/* For n null-terminated strings. */
	tmp = characters;
	for (i = 0; i < n; ++i) {
		while ('\0' != *tmp) ++tmp;
		++tmp;
	}

	/* Beginning of string. */
	return tmp;
}

blender_file_block*
get_object(const char *name, int16_t t, blender_file *bf)
{
    blender_file_block *bf_object, *bf_base;
    dna_struct *base_struct, *scene_struct, *object_struct;
    uint32_t offset;
    uint8_t len;
    int16_t type;

    /* Find scene and base.first within it. */
    len = strlen(name);
    bf_base = get_scene_block(bf->block);
    scene_struct = get_dna_struct(bf_base->sdna_idx, &bf->d);
	offset = get_offset_by_name("base.first", scene_struct, &bf->d,
        bf->header.ptr_size);
	bf_base = dereference_ptr(bf->block, bf_base->data+offset, bf->header.ptr_size);

	base_struct = get_dna_struct(bf_base->sdna_idx, &bf->d);
	print_dna_struct(base_struct, &bf->d);
	
    type = -1;
	do {
        /* Find object within Base struct. */
		offset = get_offset_by_name("object", base_struct, &bf->d,
            bf->header.ptr_size);
		bf_object = dereference_ptr(bf->block, bf_base->data+offset,
            bf->header.ptr_size);
		object_struct = get_dna_struct(bf_object->sdna_idx, &bf->d);
		print_dna_struct(object_struct, &bf->d);

        /* Looking for a certain type (Mesh, Armature etc.) */
        if (-1 != t) {
		    offset = get_offset_by_name("type", object_struct, &bf->d,
                bf->header.ptr_size);
		    type = *(int16_t*)(bf_object->data+offset);
		    printf("type: %d\n", type);
        }

        /* Compare the name of the object with what we are looking for.
         * OB<name>. */
        if (t == type) {
		    offset = get_offset_by_name("id.name", object_struct, &bf->d,
                bf->header.ptr_size);
            printf("comparing: %s\n", (char*)(bf_object->data+offset));
		    if (0 != strnstr(bf_object->data+offset, name, 24)) {
                return bf_object;
            }
        }
		offset = get_offset_by_name("next", base_struct, &bf->d, bf->header.ptr_size);
		bf_base = dereference_ptr(bf->block, bf_base->data+offset,
            bf->header.ptr_size);
	} while (0 != bf_base);

    return 0;
}

/* Calculate the offset of the index in a DNA structure. */
uint32_t
get_offset_by_index(int32_t index, const dna_struct *ds, const dna *d,
	char ptr_size)
{
	dna_struct_field *f;
	uint32_t i, size, array_size;
	const char *name, *array_start, *array_end;
	uint32_t tmp;

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

uint32_t
get_offset_by_name(const char *name, const dna_struct *ds, const dna *d,
	char ptr_size)
{
	dna_struct_field *f;
	char *buffer, *word, *reentry;
	char *sep = ".";
	int32_t idx;
	uint32_t offset;

	buffer = strdup(name);
	offset = 0;
	for (word = strtok_r(buffer, sep, &reentry); word;
		 word = strtok_r(NULL, sep, &reentry)) {
		
		printf("finding index for: %s\n", word);
		idx = get_field_index(word, ds, d);
		offset += get_offset_by_index(idx, ds, d, ptr_size);
		f = get_field_by_index(idx, ds, d);
		ds = get_dna_struct_by_type_index(f->type_idx, d);
		if (0 != ds)
			print_dna_struct(ds, d);
	}

	free(buffer);
	return offset;
}

blender_file_block*
get_scene_block(blender_file_block *first_block)
{
    blender_file_block *bfp;

	bfp = first_block;
	while (0 != strncmp(bfp->code, "ENDB", 4)) {
		if (0 == strncmp(bfp->code, "SC", 2)) {
			break;
		}
		bfp = bfp->next;
	}

    return bfp;
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

	printf("structure:%s fields:%d size:%d\n",
		get_name_ptr(d->type, dstr->type_idx), dstr->fields,
		d->byte[dstr->type_idx]);
	
	f = ((void*)dstr) + sizeof(dna_struct);
	for (j = 0; j < dstr->fields; ++j) {
		printf("\tname: %s (%d) type: %s (%d)\n",
			get_name_ptr(d->name, f->name_idx),
			f->name_idx, get_name_ptr(d->type, f->type_idx), f->type_idx);
		
		f += 1; 
	}
}

void
read_file_block(const blender_header *bh, blender_file_block *bfp,
	FILE *fp)
{

    //printf("model> reading block\n");
    fread(bfp->code, 1, 4, fp);
	if (0 == strncmp(bfp->code, "ENDB", 4)) {
		return;
	}

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


