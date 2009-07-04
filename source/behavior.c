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
#include <stdlib.h>
#include <string.h>

#include "behavior.h"

b_attribute*
b_create_attribute(const char *name, const char type, ...)
{
	b_attribute *a;
	va_list ap;
	va_start(ap, type);
	
	a = malloc(sizeof(b_attribute));
	a->name = malloc(strlen(name) * sizeof(char));
	strcpy(a->name, name);
	a->type = type;

	switch (type) {
		case 'd':
		case 'f':
		{
			double f = va_arg(ap, double);
			a->value = malloc(sizeof(double));
			memcpy(a->value, &f, sizeof(double));
			break;
		}
		default:
			break;
	}
	va_end(ap);
	return a;
}

void
b_clean_attribute(b_attribute *a)
{
	
	free(a->name);
	free(a->value);
	free(a);
	a->name = 0;
	a->value = 0;
	a = 0;
}

void
b_clean_all_attributes(b_attribute *a)
{
}

