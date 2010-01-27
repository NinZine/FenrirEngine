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

#include <string.h>

#include "actions.h"
#include "array.h"
#include "behavior.h"
#include "game.h"
#include "game_entity.h"
#include "game_helper.h"
#include "vec3.h"

#define ADD_ACTION(x) \
{#x, &action_##x##_attr, &action_##x }

#define DEF_ACTION(x) \
static void \
action_##x##_attr(b_attribute **b, unsigned int *prev_attrs); \
static void \
action_##x (void *self, b_attribute *b, unsigned int attrs)

#define DECL_ACTION_ATTR(x) \
void \
action_##x##_attr(b_attribute **b, unsigned int *prev_attrs)

#define DECL_ACTION(x) \
void \
action_##x (void *self, b_attribute *b, unsigned int attrs)

DEF_ACTION(explode);
DEF_ACTION(move);
DEF_ACTION(shoot);

static struct b_action_info action[] = {
	ADD_ACTION(explode),
	ADD_ACTION(move),
	ADD_ACTION(shoot),
};
static unsigned int actions = sizeof(action) / sizeof(struct b_action_info);

void
b_get_action(const char *name, struct b_action_info **info)
{
	int i;
	
	for (i = 0; i < actions; ++i) {
		if (0 == strcmp(name, action[i].name)) {
			*info = &action[i];
			break;
		}
	}
}

DECL_ACTION(explode)
{
	
	gh_delete_entity(((g_entity*)self)->id);
}

DECL_ACTION_ATTR(explode)
{
}

DECL_ACTION(move)
{
	b_attribute *direction,
				*speed;
	gh_rigid_body	*tmp;
	vec3		v;
	quat		rot;

	direction = b_find_attribute("direction", b, attrs);
	speed = b_find_attribute("speed", b, attrs);

	if (!direction || !speed) {
		return;
	}

	tmp = game_get_rigidbody(((g_entity*)self)->rb);
	v = *((vec3*)direction->value);
	v = vec3_normalize(&v);
	rot.w = -gh_rad2deg(atan2(v.y, v.x));
	
	v = vec3_mul(&v, *(float*)speed->value);
	tmp->linear_velocity = v;
	rot.x = rot.y = 0.f;
	rot.z = 1.f;
	tmp->rotation = quat_from_axis(&rot);
}

DECL_ACTION_ATTR(move)
{
	
	b_add_attribute(b, prev_attrs, "speed", 'f', 1.f);
}

DECL_ACTION(shoot)
{
	gh_rigid_body *me;
	g_entity *e;
	vec3 position;
	quat rotation;
	vec3 scale = {300.f, 300.f, 300.f};
	vec3 vel = {1.f, 0.f, 0.f};
	mat4 m;
	
	me = game_get_rigidbody(((g_entity*)self)->id);
	e = gh_create_entity();
	gh_array_resize((void**)&e->m, 0, sizeof(struct gh_model), 1);
	gh_create_model(e->m, S_RAY);
	e->models = 1;
	
	position = me->position;
	rotation = quat_to_axis(&me->rotation);
	m = quat_to_mat4(&me->rotation);
	vel = mat4_mul_vec3(&m, &vel, 1.f);
	vel = vec3_mul(&vel, 0.f);
	/* Spawn particle / entity */
	e->rb = gh_create_rigidbody(&position, &rotation, &scale, &vel, 0);
	/* Collides with anything but spawner,
	   spawn an explosion and delete myself
	*/
	b_add_behavior(&e->b, &e->behaviors);
	b_add_rule(&e->b[0], "collide");
	b_set_attribute(e->b[0].attr, e->b[0].attrs, "you", 3);
	//b_add_action(&e->b[0], "explode");
}

DECL_ACTION_ATTR(shoot)
{
	
	b_add_attribute(b, prev_attrs, "bullet", 'i', 0);
}
