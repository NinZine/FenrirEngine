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

/* Prototypes */
static unsigned int
rule_input_attr(b_attribute **a, unsigned int n);
static bool
rule_input(void *self, b_attribute *a, const unsigned int num_attr,
		b_attribute **out, unsigned int prev_attr, unsigned int *n);

static unsigned int
rule_see_attr(b_attribute **a, unsigned int n);
static bool
rule_see(void *self, b_attribute *a, const unsigned int num_attr,
	b_attribute **out, unsigned int prev_attr, unsigned int *n);

/* Table with all the rules */
static struct b_rule_info rule[] = {
	{"input", &rule_input_attr, &rule_input},
	{"see", &rule_see_attr, &rule_see},
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

unsigned int
rule_input_attr(b_attribute **a, unsigned int n)
{
	gh_array_resize((void**)a, n, sizeof(b_attribute), 1);
	b_create_attribute(&(*a)[n], "button", 'i', 0);
	return 1;
}

bool
rule_input(void *self, b_attribute *b, const unsigned int attrs,
	b_attribute **out, unsigned int prev_attr, unsigned int *n)
{
	b_attribute	*button;
	gh_button *tmp;
	vec3 v;
	
	button = b_find_attribute("button", b, attrs);
	if (button) {
		tmp = gh_get_input(*(int*)button->value);
		if (0 == tmp->held) {
			return false;
		}
	} else {
		return false;
	}
	
	v = vec3_normalize(&tmp->rotation);
	gh_array_resize((void**)out, prev_attr, sizeof(b_attribute), 1);
	b_create_attribute(&(*out)[prev_attr], "direction", 'v', v);
	*n += 1;
	return true;
}

unsigned int
rule_see_attr(b_attribute **a, unsigned int n)
{

	gh_array_resize((void**)a, n, sizeof(b_attribute), 2);
	b_create_attribute(&(*a)[n], "distance", 'f', 20.f);
	b_create_attribute(&(*a)[n+1], "you", 'e', 0);
	
	return 2;
}

bool
rule_see(void *self, b_attribute *b, const unsigned int num_attr,
	b_attribute **out, unsigned int prev_attr, unsigned int *n)
{
	b_attribute *distance,
				*you;
	vec3 v = {0.f, 1.f, 0.f};

	distance = b_find_attribute("distance", b, num_attr);
	you = b_find_attribute("you", b, num_attr);

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

	gh_array_resize((void**)out, prev_attr, sizeof(b_attribute), 1);
	b_create_attribute(&(*out)[prev_attr], "direction", 'v', v);
	*n += 1;
	return true;
}

