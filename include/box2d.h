#ifndef _BOX2D_H_
#define _BOX2D_H_

#include <math.h>
#include <stdbool.h>

typedef struct box2d {
	float_t x1,
			y1,
			x2,
			y2;
} box2d;

bool box2d_overlap(const box2d *box1, const box2d *box2);

#endif

