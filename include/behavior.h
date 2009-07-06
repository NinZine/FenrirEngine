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

#ifndef _BEHAVIOR_H_
#define _BEHAVIOR_H_

#include <stdbool.h>

typedef struct b_attribute {
	char *name;
	char type;
	void *value;
} b_attribute;

typedef bool (*b_rule)(void *self, b_attribute *a, const unsigned int num_attr,
	b_attribute **out, unsigned int prev_attr, unsigned int *n);
typedef void (*b_action)(void *self, b_attribute *a, unsigned int n);

typedef struct b_behavior {
	b_attribute		*rule_attr;
	b_attribute		*action_attr;
	unsigned int	num_rule_attr,
					num_action_attr,
					num_rules;
	
	b_rule *rule;
	b_action action;
} b_behavior;

bool			b_add_action(b_behavior *b, const char *name);
bool			b_add_rule(b_behavior *b, const char *name);
void			b_create_attribute(b_attribute *a, const char *name,
					const char type, ...);
void			b_create_behavior(b_behavior **b);
void			b_clean_attribute(b_attribute *a);
void			b_exec(void *self, b_behavior *b);
b_attribute*	b_find_attribute(const char *name, b_attribute *b,
					const unsigned int n);
void			b_set_attribute(b_attribute *out, ...);

#endif
