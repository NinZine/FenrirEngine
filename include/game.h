/*-
 * License
 */

#ifndef _GAME_H_
#define _GAME_H_

#include "game_helper.h"
#include "rendering.h"

g_entity* gh_create_entity();
uint32_t gh_create_rigidbody(vec3 *position, quat *rotation, vec3 *scale,
							 vec3 *vel, vec3 *ang_vel);
void gh_delete_entity(uint32_t id);
struct g_entity* game_get_entity(uint32_t id);
struct gh_rigid_body* game_get_rigidbody(uint32_t id);
void game_initialize();
//void game_input(struct gh_input *gi);
void game_render(struct r_gl_buffer *buffer);
void game_update();

#endif /* !_GAME_H_ */

