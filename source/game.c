/*-
 * License
 */

#include <assert.h>
#include <err.h>
#include <stdlib.h>

#include "game.h"
#include "game_helper.h"

static struct gh_state state_current = {0,0};
static struct gh_state state_previous = {0,0};
static gh_time game_time = {0, 0, 0, 0, 0, 1.f / 2.f};

static void game_interpolate_states(struct gh_state *out,
		struct gh_state *curr, struct gh_state *prev, float_t t);
static void game_render_state(struct gh_state *src);

void
game_initialize()
{
	vec3 pos = {25.0f, 100.0f, 25.0f};
	struct r_color color = {1.0f, 0.5f, 0.5f, 0.5f};
	int i;

	if (0 == state_current.count) {
		state_current.count = state_previous.count = 5;

		state_current.object = malloc(
				sizeof(struct gh_rigid_body) * state_current.count);
		
		/* Set some positions */
		for (i = 0; i < state_current.count; ++i) {
			
			vec3 position = {0.f, i * 5.f, 0.f};

			state_current.object[i].position = position;
		}

		gh_copy_state(&state_previous, &state_current);
	}

	r_enable_light(pos);
	r_setup_ambient_light(color);
	r_setup_diffuse_light(color);
}

void
game_interpolate_states(struct gh_state *out, struct gh_state *curr,
		struct gh_state *prev, float_t t)
{
	int i;

	if (out->count != curr->count != prev->count) {
		err(2, "gh_state is not the same size");
	}

	for (i = 0; i < curr->count; ++i) {
		
		out->object[i].position = vec3_lerp(&curr->object[i].position,
				&prev->object[i].position, t);
	}
}

void
game_render(struct r_gl_buffer *buffer)
{
	static struct gh_state tmp = {};

	if (0 == tmp.count) {
		gh_copy_state(&tmp, &state_current);
	}

	r_bind_buffers(buffer);
	glViewport(0, 0, buffer->width, buffer->height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	r_setup_orthogonal_view(buffer->width, buffer->height);
	//glRotatef(90.0f, 0.0f, 0.0f, -1.0f); // For landscape mode
	
	// Clear buffers
	glEnable(GL_DEPTH_TEST);
	//glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glClearColor(.0f, .0f, .0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	float_t interpolate = 1.0f - game_time.accumulator/game_time.timestep;
	game_interpolate_states(&tmp, &state_current, &state_previous,
			interpolate);
	
	static const GLfloat col[] = {0.75f, 0.75f, 0.75f, 1.f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, col); /* Only effective without glEnable(GL_COLOR_MATERIAL) */

	game_render_state(&tmp);
	
	glRotatef(180.f, 0.f, 0.f, 1.f);
	/*glTranslatef(w.x, w.y, w.z);*/
	r_render_circle(20);
}

void
game_render_state(struct gh_state *src)
{
	int i;

	for (i = 0; i < src->count; ++i) {

		glColor4f(0.6f, 0.6f, 0.6f, 1.f);
		glPushMatrix();
		glTranslatef(src->object[i].position.x, src->object[i].position.y,
				src->object[i].position.z);
		/*glRotatef(w.y, 1.f, 0.f, 0.f);*/
		r_render_cube(50);
		glPopMatrix();
	}
}

void
game_update()
{
	game_time.now = gh_time_elapsed();
	game_time.delta = game_time.now - game_time.absolute;
	
	if (game_time.delta <= 0.0f) {
		return;
	}
	
	game_time.absolute = game_time.now;
	game_time.accumulator += game_time.delta;
	
	while (game_time.accumulator >= game_time.timestep) {
		
		/* Update game state */
		/*prev = curr;
		curr.x += 0.f;
		curr.y += 5.f;
		curr.z += 0.f;*/
		
		game_time.accumulator -= game_time.timestep;
		game_time.frame += 1;
	}
}

