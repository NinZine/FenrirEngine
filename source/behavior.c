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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "actions.h"
#include "array.h"
#include "behavior.h"
#include "game_entity.h"
#include "rules.h"
#include "vec3.h"

static void b_init_attribute_value(void **value, bool alloc, size_t bytes,
	void *data);
static void b_parse_attribute(b_attribute *a, bool modify, va_list *ap);

bool
b_add_rule(b_behavior *b, const char *name)
{
	struct b_rule_info *r = 0;
	
	b_get_rule(name, &r);
	if (0 == r) {
		return false;
	}

	if (r->create_default_attr) {
		b->num_rule_attr += r->create_default_attr(&b->rule_attr,
			b->num_rule_attr);
	}
	gh_array_resize((void**)&b->rule, b->num_rules, sizeof(b_rule), 1);
	b->rule[b->num_rules] = r->rule;
	b->num_rules += 1;
	return true;
}

bool
b_add_action(b_behavior *b, const char *name)
{
	struct b_action_info *a = 0;
	
	b_get_action(name, &a);
	if (0 == a) {
		return false;
	}
	
	if (a->create_default_attr) {
		b->num_action_attr += a->create_default_attr(&b->action_attr,
			b->num_action_attr);
	}
	b->action = a->action;
	return true;
}

void
b_create_attribute(b_attribute *a, const char *name, const char type, ...)
{
	va_list ap;

	a->name = malloc(strlen(name) * sizeof(char));
	strcpy(a->name, name);
	a->type = type;
	va_start(ap, type);
	b_parse_attribute(a, false, &ap);
	va_end(ap);
}

void
b_create_behavior(b_behavior **b)
{
	
	*b = malloc(sizeof(b_behavior));
	bzero(*b, sizeof(b_behavior));
}

void
b_clean_attribute(b_attribute *a)
{
	
	free(a->name);
	free(a->value);
	a->name = 0;
	a->value = 0;
}

void
b_exec(void *self, b_behavior *b)
{
	int i;
	unsigned int extra_attr = 0;
	bool rules_passed = true;

	for (i = 0; i < b->num_rules; ++i) {
		if (false == b->rule[i](self, b->rule_attr, b->num_rule_attr,
			&b->action_attr, b->num_action_attr, &extra_attr)) {
			rules_passed = false;
			break;
		}
	}

	if (rules_passed && 0 != b->action) {
		b->action(self, b->action_attr, b->num_action_attr + extra_attr);
	}

	/* Delete extra attributes */
	for (i = 0; i < extra_attr; ++i) {
		b_clean_attribute(&b->action_attr[b->num_action_attr + i]);
		gh_array_resize((void**)b->action_attr, b->num_action_attr + extra_attr,
			sizeof(b_attribute), -extra_attr);
	}
}

b_attribute*
b_find_attribute(const char *name, b_attribute *b, const unsigned int n)
{
	int i;
	b_attribute *tmp;

	tmp = 0;
	for (i = 0; i < n; ++i) {
		if (0 == strcmp(name, b[i].name)) {
			tmp = &b[i];
			break;
		}
	}

	return tmp;
}

void
b_init_attribute_value(void **value, bool modify, size_t bytes, void *data)
{
	
	if (!modify) {
		*value = malloc(bytes);
	}
	memcpy(*value, data, bytes);
}

void
b_parse_attribute(b_attribute *a, bool modify, va_list *ap)
{

	switch (a->type) {
		case 'e':
		{
			g_entity *d = va_arg(*ap, g_entity*);
			b_init_attribute_value(&a->value, modify, sizeof(d), (void*)&d);
			break;
		}
		case 'd':
		case 'f':
		{
			float d = (float)va_arg(*ap, double);
			b_init_attribute_value(&a->value, modify, sizeof(d), (void*)&d);
			break;
		}
		case 'i':
		{
			int d = (int)va_arg(*ap, int);
			b_init_attribute_value(&a->value, modify, sizeof(d), (void*)&d);
			break;
		}
		case 'v':
		{
			vec3 d = va_arg(*ap, vec3);
			b_init_attribute_value(&a->value, modify, sizeof(d), (void*)&d);
		}
		default:
			break;
	}
}

void
b_set_attribute(b_attribute *a, ...)
{
	va_list ap;

	va_start(ap, a);
	b_parse_attribute(a, true, &ap);
	va_end(ap);
}

