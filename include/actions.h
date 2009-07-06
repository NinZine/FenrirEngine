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

#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include "behavior.h"

struct b_action_info {
	const char *name;
	void (*create_default_attr)(b_attribute **a, unsigned int *counter);
	b_action action;
};

void b_get_action(const char *name, struct b_action_info **info);

#endif
