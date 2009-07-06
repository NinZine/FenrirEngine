/*-
 * License
 */

#ifndef _GAME_H_
#define _GAME_H_

#include "game_helper.h"
#include "rendering.h"

void game_initialize();
//void game_input(struct gh_input *gi);
void game_render(struct r_gl_buffer *buffer);
void game_update();

#endif /* !_GAME_H_ */

