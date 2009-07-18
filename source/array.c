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

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

void
gh_array_resize(void **array, unsigned int num_elements, size_t size_element,
				int plus_elements)
{
	int		bytes,
			new_size;
	void	*tmp;
	
	new_size = num_elements + plus_elements;
	bytes = size_element * (new_size);
	
	if (0 == new_size && num_elements > 0) {
		free(*array);
		*array = 0;
		return;
	} else if (0 == num_elements) {
		tmp = calloc(new_size, size_element);
	} else if (new_size > 0) {
		tmp = realloc(*array, bytes);
	}
	
	if (0 == tmp) {
		err(1, "Failed to resize array!");
	}
	*array = tmp;
}

