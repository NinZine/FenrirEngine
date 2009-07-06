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

#ifndef _RULES_H_
#define _RULES_H_

#include "behavior.h"

struct b_rule_info {
	const char *name;
	void (*create_default_attr)(b_attribute **a, unsigned int *n);
	b_rule rule;
};

void b_get_rule(const char *name, struct b_rule_info **info);

#endif
