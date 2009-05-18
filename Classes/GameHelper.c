/*
 *  GameHelper.c
 *  Demo2
 *
 *  Created by Andreas Kröhnke on 5/14/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#include <sys/time.h>

#include "GameHelper.h"

float_t
time_elapsed()
{
	static struct timeval start = {0,0};
	struct timeval counter;
	
	if (start.tv_sec == 0 && start.tv_usec == 0)
	{
		gettimeofday(&start, 0);
		return 0.f;
	}
	
	gettimeofday(&counter, 0);
	counter.tv_sec = counter.tv_sec - start.tv_sec;
	counter.tv_usec = counter.tv_usec - start.tv_usec;
	
	float_t tmp = ((counter.tv_sec * 1000000) + counter.tv_usec) / 1000000.f;
    return tmp;
}
