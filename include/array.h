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

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <ctype.h>

void	gh_array_resize(void **array, unsigned int num_elements,
						size_t size_element, int plus_elements);

#endif
