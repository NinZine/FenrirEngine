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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "array.h"
#include "behavior.h"
#include "game_entity.h"
#include "game_helper.h"
#include "rules.h"
#include "vec3.h"

#define ADD_RULE(x) \
{#x, &rule_##x##_attr, &rule_##x }

#define DEF_RULE(x) \
static void \
rule_##x##_attr (b_attribute **, unsigned int *); \
static bool \
rule_##x (void *self, b_attribute *a, const unsigned int attrs, \
			 b_attribute **out, unsigned int *prev_attr)

#define DECL_RULE_ATTR(x) \
void \
rule_##x##_attr(b_attribute **a, unsigned int *n)

#define DECL_RULE(x) \
bool \
rule_##x (void *self, b_attribute *b, const unsigned int attrs, \
			 b_attribute **out, unsigned int *prev_attr)

/* Prototypes */
DEF_RULE(collide);
DEF_RULE(input);
DEF_RULE(see);

/* Table with all the rules */
static struct b_rule_info rule[] = {
	ADD_RULE(collide),
	ADD_RULE(input),
	ADD_RULE(see),
};

static unsigned int num_rules = sizeof(rule) / sizeof(struct b_rule_info);

void
b_get_rule(const char *name, struct b_rule_info **info)
{
	int i;

	for (i = 0; i < num_rules; ++i) {
		if (0 == strcmp(name, rule[i].name)) {
			*info = &rule[i];
			break;
		}
	}
}

DECL_RULE_ATTR(collide)
{
	
	b_add_attribute(a, n, "bounce", 'b', false);
	b_add_attribute(a, n, "you", 'e', 0);
}

DECL_RULE(collide)
{
	b_attribute	*tmp;
	g_entity	*me,
				*you;
	mat4 tf1;
	vec3 edge[4] = {
		{1.f, 0.f, 0.f},
		{0.f, 1.f, 0.f},
		{1.f, 0.f, 0.f},
		{0.f, 1.f, 0.f},
	}; /* TODO: Find edges, this should be done at load. */
	vec3 point1[4] = {
		{-0.5f, -0.5f, 0.f},
		{ 0.5f, -0.5f, 0.f},
		{-0.5f,  0.5f, 0.f},
		{ 0.5f,  0.5f, 0.f},
	}; /* TODO: This is vertex data */
	mat4 tf2;
	vec3 point2[4] = {
		{-0.5f, -0.5f, 0.f},
		{ 0.5f, -0.5f, 0.f},
		{-0.5f,  0.5f, 0.f},
		{ 0.5f,  0.5f, 0.f},
	};
	float_t min_dist = 0.f;
	int axis = -1;
	
	tmp = b_find_attribute("you", b, attrs);
	if (tmp) {
		you = *(g_entity**)tmp->value;
		if (0 == you) {
			return false;
		}
	} else {
		return false;
	}
	
	me = (g_entity*)self;
	gh_build_mat4(me->rb, &tf1);
	gh_transform_edges(&tf1, &edge[0], 2);
	gh_transform_vec3(&tf1, point1, 4);
	gh_build_mat4(you->rb, &tf2);
	gh_transform_edges(&tf2, &edge[2], 2);
	gh_transform_vec3(&tf2, point2, 4);
	
	if (true == gh_collides(edge, 4, point1, point2, &min_dist, &axis)) {
		vec3 trans = edge[axis];
		vec3 dist;
		
		dist = vec3_sub(&me->rb->position, &you->rb->position);
		if (vec3_dot(&dist, &trans) < 0) {
			trans.x = -trans.x; trans.y = -trans.y; trans.z = -trans.z;
		}
		
		/* Project out of collision */
		trans = vec3_mul(&trans, min_dist+0.05f);
		trans = vec3_add(&me->rb->position, &trans);
		me->rb->position = trans;
		bzero(&me->rb->linear_velocity, sizeof(vec3));
		printf("Collision: %x and %x on axis %d dist (%.2f,%.2f)\n", me->rb, you->rb, axis, trans.x, trans.y);
		//b_add_attribute(out, prev_attr, "direction", 'v', v);
		return true;
	}
	
	return false;
}

DECL_RULE_ATTR(input)
{
	b_add_attribute(a, n, "button", 'i', 0);
}

DECL_RULE(input)
{
	b_attribute	*button;
	gh_button *tmp;
	vec3 v;
	
	button = b_find_attribute("button", b, attrs);
	if (button) {
		tmp = gh_get_input(*(int*)button->value);
		if (0 == tmp || 0 == tmp->held) {
			return false;
		}
	} else {
		return false;
	}
	
	v = vec3_normalize(&tmp->rotation);
	b_add_attribute(out, prev_attr, "direction", 'v', v);
	return true;
}

DECL_RULE_ATTR(see)
{

	b_add_attribute(a, n, "distance", 'f', 20.f);
	b_add_attribute(a, n, "you", 'e', 0);
}

DECL_RULE(see)
{
	b_attribute *distance,
				*you;
	vec3 v = {0.f, 1.f, 0.f};

	distance = b_find_attribute("distance", b, attrs);
	you = b_find_attribute("you", b, attrs);

	if (distance && you) {
		float len;
		float dist = *((float*)distance->value);
		g_entity *y = *(g_entity**)you->value;
		g_entity *e = (g_entity*)self;
		
		v = vec3_sub(&y->rb->position,
			&e->rb->position);
		len = vec3_length(&v);

		if (len > dist) {
			return false;
		}
	} else {
		return false;
	}

	b_add_attribute(out, prev_attr, "direction", 'v', v);
	return true;
}

