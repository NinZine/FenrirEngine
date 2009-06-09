/*-
 * License
 */

#include "box2d.h"

bool
box2d_overlap(const box2d *box1, const box2d *box2)
{
	static const float_t t = 0.001f;

	if (box1->x1 > box2->x2 + t) {
		return false;
	} else if (box1->x2 < box2->x1 + t) {
		return false;
	} else if (box1->y1 > box2->y2 + t) {
		return false;
	} else if (box1->y2 < box2->y1 + t) {
		return false;
	}

	return true;
}

