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
	
	struct b_attribute *next, *prev;
} b_attribute;

typedef bool (*b_rule)(const b_attribute *a, b_attribute *out);

struct b_behavior {
	b_attribute *rule_attribute;
	b_attribute *action_attribute;
	b_rule *rule;
	void (*action)(const b_attribute *a);
};

b_attribute*	b_create_attribute(const char *name, const char type, ...);
void			b_clean_attribute(b_attribute *a);
void			b_clean_all_attributes(b_attribute *a);

#endif
