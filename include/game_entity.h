/*-
	 Copyright (C) 2009 Andreas Kroehnke <ninzine@indraz.com>
	 
	 This program is free software; you can redistribute it and/or
	 modify it under the terms of the GNU General Public License
	 as published by the Free Software Foundation; either version 2
	 of the License, or (at your option) any later version.
	 
	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _GAME_ENTITY_H_
#define _GAME_ENTITY_H_

#include <stdint.h>

#include "behavior.h"
#include "quat.h"
#include "vec3.h"

enum gh_type {
	T_SOLID,
	T_MOVEABLE,
};

enum gh_shape {
	S_CIRCLE,
	S_POLYGON,
};

struct gh_rigid_body {
	int32_t id;
	vec3	position;
	quat	rotation;
	vec3	scale;
	vec3	linear_velocity;
	vec3	angular_velocity;
};

struct gh_model {
	/*char *filename;
	 int texture;*/
	enum gh_shape shape;
	vec3	*vertex,
			*edge;
	uint8_t vertices,
			edges;
};

typedef struct g_entity {
	b_behavior *b;
	struct gh_rigid_body *rb;
	struct gh_model	*m;
	
	unsigned int behaviors,
				 models;
} g_entity;

#endif
