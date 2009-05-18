/*
 *  GameHelper.h
 *  Demo2
 *
 *  Created by Andreas Kröhnke on 5/14/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#ifndef _GAMEHELPER_H_
#define _GAMEHELPER_H_

#include <math.h>

typedef struct Time {
	float_t absolute,
			accumulator,
			delta,
			frame,
			now,
			timestep;
} Time;

float_t time_elapsed();

#endif /* !_GAMEHELPER_H_ */
