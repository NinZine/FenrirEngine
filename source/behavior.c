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

static void	b_create_attribute(b_attribute *a, const char *name,
				const char type, va_list *ap);
static void b_init_attribute_value(void **value, bool alloc, size_t bytes,
				void *data);
static void b_parse_attribute(b_attribute *a, bool modify, va_list *ap);


bool
b_add_action(b_behavior *b, const char *name)
{
	struct b_action_info *a = 0;
	
	b_get_action(name, &a);
	if (0 == a) {
		return false;
	}
	
	if (a->create_default_attr) {
		a->create_default_attr(&b->attr, &b->attrs);
	}
	b->action = a->action;
	return true;
}

void
b_add_attribute(b_attribute **a, unsigned int *n, const char *name,
	const char type, ...)
{
	b_attribute *tmp;
	va_list ap;
	
	va_start(ap, type);
	tmp = b_find_attribute(name, *a, *n);
	if (0 == tmp) {
		gh_array_resize((void**)a, *n, sizeof(b_attribute), 1);
		*n += 1;
		b_create_attribute(&(*a)[*n-1], name, type, &ap);
	} else {
		b_parse_attribute(tmp, true, &ap);
	}
	
	va_end(ap);
}

void
b_add_behavior(b_behavior **b, unsigned int *n)
{
	
	gh_array_resize((void**)b, *n, sizeof(b_behavior), 1);
	*n += 1;
}

bool
b_add_rule(b_behavior *b, const char *name)
{
	struct b_rule_info *r = 0;
	
	b_get_rule(name, &r);
	if (0 == r) {
		return false;
	}
	
	if (r->create_default_attr) {
		r->create_default_attr(&b->attr, &b->attrs);
	}
	gh_array_resize((void**)&b->rule, b->num_rules, sizeof(b_rule), 1);
	b->rule[b->num_rules] = r->rule;
	b->num_rules += 1;
	return true;
}

void
b_create_attribute(b_attribute *a, const char *name, const char type,
	va_list *ap)
{
	
	a->name = malloc(strlen(name) * sizeof(char));
	strcpy(a->name, name);
	a->type = type;
	b_parse_attribute(a, false, ap);
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
	unsigned int extra_attr = b->attrs;
	bool rules_passed = true;

	for (i = 0; i < b->num_rules; ++i) {
		if (false == b->rule[i](self, &b->attr, &b->attrs)) {
			rules_passed = false;
		}
	}

	if (rules_passed && 0 != b->action) {
		b->action(self, b->attr, b->attrs);
	}

	/* Delete extra attributes */
	extra_attr = b->attrs - extra_attr;
	for (i = b->attrs-1; i < b->attrs - 1 + extra_attr; ++i) {
		b_clean_attribute(&b->attr[i]);
	}
	if (extra_attr > 0) {
		gh_array_resize((void**)b->attr, b->attrs,
			sizeof(b_attribute), -extra_attr);
		b->attrs -= extra_attr;
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
		case 'b':
		{
			bool d = (bool)va_arg(*ap, int);
			b_init_attribute_value(&a->value, modify, sizeof(d), (void*)&d);
			break;
		}
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
b_set_attribute(b_attribute *a, const unsigned int n, const char *name, ...)
{
	b_attribute *tmp;
	va_list ap;

	va_start(ap, name);
	tmp = b_find_attribute(name, a, n);
	if (0 != tmp) {
		b_parse_attribute(tmp, true, &ap);
	}
	va_end(ap);
}

