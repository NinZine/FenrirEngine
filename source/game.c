/*-
 * License
 */

#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <strings.h>

#include "game.h"
#include "game_helper.h"

static vec3 camera_current = {0.f, 0.f, 200.f};
static vec3 camera_previous = {0.f, 0.f, 200.f};
static vec3 light_position = {0.0f, 0.0f, 25.0f};
static bool	touch = false; /* FIXME: Temporary */
static struct gh_state state_current = {0,0};
static struct gh_state state_previous = {0,0};
static gh_time game_time = {0, 0, 0, 0, 0, 1.f / 2.f};

static void game_input_handle();
static void game_interpolate_states(struct gh_state *out,
		struct gh_state *curr, struct gh_state *prev, float_t t);
static void game_render_state(struct gh_state *src);
static void game_update_state(struct gh_state *curr, struct gh_state *prev);

void
game_initialize()
{
	struct r_color color = {1.0f, 0.5f, 0.5f, 0.5f};
	int i;

	if (0 == state_current.count) {
		state_current.count = state_previous.count = 5;

		state_current.object = malloc(
				sizeof(struct gh_rigid_body) * state_current.count);
		bzero(state_current.object,
				sizeof(struct gh_rigid_body) * state_current.count);
		
		/* Set some positions */
		for (i = 0; i < state_current.count; ++i) {
			
			vec3 position = {
				0.f,
				(state_current.count * -20.f) + (i * 40.f),
				0.f};
			vec3 velocity = {rand() % 10 - 5.f, rand() % 10 - 5.f, 0.f};

			state_current.object[i].position = position;
			//state_current.object[i].linear_velocity = velocity;
		}

		gh_copy_state(&state_previous, &state_current, true);
	}

	r_enable_light(0);
	r_setup_ambient_light(0, color);
	r_setup_diffuse_light(0, color);
	r_set_light_position(0, &light_position);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.008f);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.001f);
	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 5.f);
}

void
game_input_handle()
{

	camera_previous = camera_current;
	if (true == touch) {
		//camera_current.z += 5.f;
		light_position.z -= 5.0f;
		r_set_light_position(0, &light_position);
	}
}

void
game_input_touch_begin(vec3 position)
{
	
	touch = true;
}

void
game_input_touch_end()
{

	touch = false;
}

void
game_interpolate_states(struct gh_state *out, struct gh_state *curr,
		struct gh_state *prev, float_t t)
{
	int i;

	if (out->count != curr->count || out->count != prev->count) {
		err(1, "gh_state is not the same size");
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
	static float_t aspect_ratio = 0.f;
	vec3 camera_tmp;
	float_t interpolate = 1.0f - game_time.accumulator/game_time.timestep;

	/* First frame */
	if (0 == tmp.count) {
		gh_copy_state(&tmp, &state_current, true);

		aspect_ratio = (float_t)buffer->width / (float_t)buffer->height;
	}

	/* Setup buffers and view */
	r_bind_buffers(buffer);
	glViewport(0, 0, buffer->width, buffer->height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	r_setup_perspective_view(60.0f, aspect_ratio,
			0.01f, 1000.f);
	camera_tmp = vec3_lerp(&camera_current, &camera_previous, interpolate);
	glTranslatef(-camera_tmp.x, -camera_tmp.y, -camera_tmp.z);
	//r_setup_orthogonal_view(buffer->width, buffer->height);
	//glRotatef(90.0f, 0.0f, 0.0f, -1.0f); // For landscape mode
	
	/* Clear buffers */
	glClearColor(1.0f, 1.0f, 1.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	
	/* World space */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	game_interpolate_states(&tmp, &state_current, &state_previous,
			interpolate);
	
	game_render_state(&tmp);
	
	/* Make sure there is no error in OpenGL stuff */
	assert(glGetError() == GL_NO_ERROR);
}

void
game_render_state(struct gh_state *src)
{
	static const GLfloat color[] = {1.f, 0.f, 0.f, 1.f};
	static int16_t rotate = 0;
	int i;

	for (i = 0; i < src->count; ++i) {

		glColor4f(0.9, 0.4, 0.4, 1.f);
		glPushMatrix();
		/* Only effective without glEnable(GL_COLOR_MATERIAL) */
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
		
		glEnable(GL_COLOR_MATERIAL);
		glTranslatef(src->object[i].position.x, src->object[i].position.y,
				src->object[i].position.z);
		glRotatef(rotate++, 0.f, 1.f, 0.f);
		r_render_cube(20);
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
		
		game_input_handle();
		game_update_state(&state_current, &state_previous);
		
		game_time.accumulator -= game_time.timestep;
		game_time.frame += 1;
	}
}

void
game_update_state(struct gh_state *curr, struct gh_state *prev)
{
	int16_t i;

	gh_copy_state(prev, curr, false);

	for (i = 0; i < curr->count; ++i) {
		curr->object[i].position = vec3_add(&curr->object[i].position,
				&curr->object[i].linear_velocity);
	}
}

