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
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "behavior.h"
#include "game.h"
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
rule_##x (void *self, b_attribute **a, unsigned int *attrs)

#define DECL_RULE_ATTR(x) \
void \
rule_##x##_attr(b_attribute **a, unsigned int *n)

#define DECL_RULE(x) \
bool \
rule_##x (void *self, b_attribute **b, unsigned int *attrs)

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
	b_add_attribute(a, n, "you", 'u', 0);
}

DECL_RULE(collide)
{
	b_attribute	*tmp;
	g_entity	*me,
				*you;
	vec3		direction;
	float_t distance = 0.f;
	bool collides = false;
	
	tmp = b_find_attribute("you", *b, *attrs);
	if (tmp) {
		you = game_get_entity(*(uint32_t*)tmp->value);
		if (0 == you) {
			return false;
		}
	} else {
		return false;
	}
	
	me = (g_entity*)self;
	
	if (0 == me->models || 0 == me->rb || 0 == you->models || 0 == you->rb) {
		return false;
	}
	
	/* TODO: Circle poly collision
	   Find closest point to center of circle
	   Create vector and normalize
	   Project polygon on that vector
	   Project circle on that vector (vector * (pos +/-radius))
	*/
	
	/* TODO: Circle ray collsion */
	/* TODO: Ray poly collision */
	/* TODO: Consider velocity
	   Project velocity on axis
	   Add to min/max of this polygon
	   Test for overlap again
	*/
	
	collides = gh_collides(me, you, &direction, &distance);
	if (true == collides) {
		gh_rigid_body	*me_rb,
						*you_rb;
		
		me_rb = game_get_rigidbody(me->rb);
		you_rb = game_get_rigidbody(you->rb);
		
		/* Project out of collision */
		direction = vec3_mul(&direction, distance+0.05f);
		me_rb->position = vec3_add(&me_rb->position, &direction);
		bzero(&me_rb->linear_velocity, sizeof(vec3));
		printf("Collision: %x and %x direction (%.2f,%.2f)\n", me_rb, you_rb, direction.x, direction.y);
		b_add_attribute(b, attrs, "direction", 'v', direction);
		collides = true;
	}
	
	return collides;
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
	
	button = b_find_attribute("button", *b, *attrs);
	if (button) {
		tmp = gh_get_input(*(int*)button->value);
		if (0 == tmp || 0 == tmp->held) {
			return false;
		}
	} else {
		return false;
	}
	
	v = vec3_normalize(&tmp->rotation);
	b_add_attribute(b, attrs, "direction", 'v', v);
	return true;
}

DECL_RULE_ATTR(see)
{

	b_add_attribute(a, n, "distance", 'f', 20.f);
	b_add_attribute(a, n, "you", 'u', 0);
}

DECL_RULE(see)
{
	b_attribute *distance,
				*you;
	vec3 v = {0.f, 1.f, 0.f};

	distance = b_find_attribute("distance", *b, *attrs);
	you = b_find_attribute("you", *b, *attrs);

	if (distance && you) {
		float len;
		float dist = *((float*)distance->value);
		g_entity *y = game_get_entity(*(uint32_t*)you->value);
		g_entity *e = (g_entity*)self;
		gh_rigid_body	*me_rb,
						*you_rb;
		
		me_rb = game_get_rigidbody(e->rb);
		you_rb = game_get_rigidbody(y->rb);
		v = vec3_sub(&you_rb->position,
			&me_rb->position);
		len = vec3_length(&v);

		if (len > dist) {
			return false;
		}
	} else {
		return false;
	}

	b_add_attribute(b, attrs, "direction", 'v', v);
	return true;
}

