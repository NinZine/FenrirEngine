/*-
 * License
 */

#ifndef _GAME_H_
#define _GAME_H_

#include "rendering.h"

void game_initialize();
void game_input_touch_begin(vec3 position);
void game_input_touch_end();
void game_render(struct r_gl_buffer *buffer);
void game_update();

#endif /* !_GAME_H_ */

