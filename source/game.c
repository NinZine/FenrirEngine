/*-
 * License
 */

#include <sys/time.h>

#include <alloca.h>
#include <assert.h>
#if __IPHONE__ == 1
#include <err.h>
#endif /* !__IPHONE__ */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "box2d.h"
#include "game.h"
#include "game_helper.h"
#include "mat4.h"
#include "quat.h"

#if __NDS__ == 1
void inline
err(int n, char *fmt, ...)
{
	va_list msg;
	char *tmp = alloca((strlen(fmt) + 2) * sizeof(char));

	va_start(msg, fmt);
	strcat(tmp, fmt);
	strcat(tmp, "\n");
	vprintf(tmp, msg);
	va_end(msg);
}
#endif /* !__NDS__ */

struct gh_input_queue {
	int capacity;
	int count;
	struct gh_input *queue;
};

static vec3 camera_current = {0.f, 0.f, 200.f};
static vec3 camera_previous = {0.f, 0.f, 200.f};
static vec3 light_position = {0.0f, -1.0f, 0.0f};
static struct gh_state state_current = {0,0};
static struct gh_state state_previous = {0,0};
static gh_time game_time = {0, 0, 0, 0, 0, 1.f / 20.f};
static struct gh_input_queue input_queue;

static void game_initialize_light();
static void game_input_handle();
static void game_interpolate_states(struct gh_state *out,
	struct gh_state *curr, struct gh_state *prev, float_t t);
static void game_render_state(struct gh_state *src);
static void game_resolve_collisions(struct gh_state *curr,
	struct gh_state *prev);
static void game_update_state(struct gh_state *curr, struct gh_state *prev);

void
game_initialize()
{
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
			quat rotation = {0.f, 0.f, 1.f, 0.f};

			state_current.object[i].position = position;
			state_current.object[i].rotation = quat_from_axis(&rotation);
			//state_current.object[i].linear_velocity = velocity;
		}

		gh_copy_state(&state_previous, &state_current, true);
	}

	input_queue.capacity = 8;
	input_queue.count = 0;
	input_queue.queue = (struct gh_input*)malloc(
		sizeof(struct gh_input) * input_queue.capacity);
	game_initialize_light();
	
	/* Matrix test */
	printf("Start matrix test\n");
	mat4 m1, m2, m3, m4;
	quat q = {45.f, 0.f, 1.f, 0.f};
	
	mat4_identity(&m1);
	mat4_copy(&m1, &m2);
	mat4_copy(&m2, &m3);
	mat4_copy(&m3, &m4);
	
	mat4_print(&m1);
	mat4_print(&m2);
	mat4_print(&m3);
	mat4_print(&m4);
	
	float_t det = mat4_determinant(&m1);
	printf("Determinant: %.2f\n", det);
	
	mat4_reset(&m2);
	mat4_print(&m2);
	
	mat4_rotate(&m1, 45.f, 1.f, 0.f, 1.f);
	mat4_print(&m1);
	
	q = quat_from_axis(&q);
	quat_to_mat4(&q, &m3);
	mat4_print(&m3);
	
	printf("End matrix test\n");
	/* !Matrix test */
}

void
game_initialize_light()
{
	struct r_color color = {1.0f, 1.f, 1.f, 1.f};
	struct r_color material = {1.0f, 0.0f, 0.0f, 0.0f};

	//r_enable_light(0);
	r_set_light_position(0, &light_position);
	r_setup_ambient_light(0, material);
	r_setup_diffuse_light(0, color);

	/* Set default material for everything, need to do this for DS */
	r_set_material(GL_AMBIENT, material);
	r_set_material(GL_DIFFUSE, material);
	r_set_material(GL_EMISSION, material);
	r_set_material(GL_SPECULAR, material);
}

void
game_input_handle()
{
	camera_previous = camera_current;

	while (input_queue.count > 0) {
		--input_queue.count;
		
		struct gh_input gi = input_queue.queue[input_queue.count];
		switch (gi.type)
		{
			case GHI_MOVE:
			{
				vec3 *dir = (vec3*)gi.data;
				state_current.object[1].linear_velocity = *dir; 
				//vec3_add(&state_current.object[3].position, dir);
				free(gi.data);
				break;
			}
			case GHI_MOVE_STOP:
			{
				vec3 dir = {0.f, 0.f, 0.f};
				state_current.object[1].linear_velocity = dir;
				break;
			}
		};
	}
}

/* Puts struct on a queue */
void
game_input(struct gh_input gi)
{
	
	if (input_queue.count >= input_queue.capacity) {
		void *tmp = malloc(
			sizeof(struct gh_input) * (input_queue.capacity + 10));
		memcpy(tmp, input_queue.queue,
			sizeof(struct gh_input) * input_queue.capacity);

		free(input_queue.queue);
		input_queue.queue = tmp;
		input_queue.capacity += 10;
	}

	input_queue.queue[input_queue.count] = gi;
	++input_queue.count;
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
		
		out->object[i].position = vec3_lerp(&prev->object[i].position,
			&curr->object[i].position, t);
		
		/*out->object[i].rotation = quat_slerp(&curr->object[i].rotation,
			&prev->object[i].rotation, t);*/
		out->object[i].rotation = quat_slerp(&prev->object[i].rotation,
			&curr->object[i].rotation, t);
	}
}

void
game_render(struct r_gl_buffer *buffer)
{
	static struct gh_state tmp = {};
	static float_t aspect_ratio = 0.f;
	vec3 camera_tmp;
	float_t interpolate = /*1.0f - */game_time.accumulator/game_time.timestep;

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
	r_setup_orthogonal_view(buffer->width, buffer->height);
	//r_setup_perspective_view(60.0f, aspect_ratio,
	//		0.01f, 1000.f);
	camera_tmp = vec3_lerp(&camera_current, &camera_previous, interpolate);
	//glTranslatef(-camera_tmp.x, -camera_tmp.y, -camera_tmp.z);
	glRotatef(90.0f, 0.0f, 0.0f, -1.0f); // For landscape mode
	
	/* Clear buffers */
	r_clear();
	r_enable_culling(GL_BACK);
#if __IPHONE__ == 1
	glEnable(GL_DEPTH_TEST);
	//glFrontFace(GL_CCW);
#elif __NDS__ == 1 /* !__IPHONE__ */
#endif /* !__NDS__ */
	
	/* World space */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	game_interpolate_states(&tmp, &state_current, &state_previous,
			interpolate);
	
	game_render_state(&tmp);
	
	/* Make sure there is no error in OpenGL stuff */
	//assert(glGetError() == GL_NO_ERROR);
}

void
game_render_state(struct gh_state *src)
{
	static const struct r_color ambient = {1.f, 0.7f, 0.7f, 0.7f};
	static const struct r_color diffuse = {1.f, 0.8f, 0.5f, 0.1f};
	int i;

	for (i = 0; i < src->count-1; ++i) {
		quat q = quat_to_axis(&src->object[i].rotation);

		glColor4f(0.9, 0.6, 0.4, 1.f);
		glPushMatrix();
		/* Only effective without glEnable(GL_COLOR_MATERIAL) */
		r_set_material(GL_AMBIENT, ambient);
		r_set_material(GL_DIFFUSE, diffuse);
		
		glEnable(GL_COLOR_MATERIAL);
		glTranslatef(src->object[i].position.x, src->object[i].position.y,
				src->object[i].position.z);
		glRotatef(q.w, q.x, q.y, q.z);
		glScalef(20, 20, 20);
		r_render_quad(20);
		glPopMatrix();
	}
	
	/* Matrix test - rotates more smoothly for some reason */
	mat4 pos, rot, sca;
	quat_to_mat4(&src->object[i].rotation, &rot);
	mat4_identity(&pos);
	pos.m[3][0] = src->object[i].position.x;
	pos.m[3][1] = src->object[i].position.y;
	pos.m[3][2] = src->object[i].position.z;
	mat4_identity(&sca);
	sca.m[0][0] = 20.f;
	sca.m[1][1] = 20.f;
	sca.m[2][2] = 20.f;
	mat4_mul(&sca, &rot, &rot);
	mat4_mul(&rot, &pos, &pos);
	
	glPushMatrix();
	glLoadMatrixf((GLfloat *)pos.m);
	r_render_quad(1);
	glPopMatrix();
	/* !Matrix test */
}

void
game_resolve_collisions(struct gh_state *curr, struct gh_state *prev)
{
	box2d box1, box2;
	int i, j;

	for (i = 0; i < curr->count; ++i) {
		/*vec3 tmp, norm;

		tmp = vec3_sub(&prev->object[i].position, &curr->object[i].position);
		norm = vec3_normalize(&tmp);

		/* AABB */
		box1.x1 = curr->object[i].position.x - (1 * 10.f); /* Left */
		box1.y1 = curr->object[i].position.y - (1 * 10.f); /* Bottom */
		box1.x2 = curr->object[i].position.x + (1 * 10.f); /* Right */
		box1.y2 = curr->object[i].position.y + (1 * 10.f); /* Top */

		/* Create two axis from x1,y1 -> x2,y1 and x1,y1 -> x1, y2 */
		/*vec3 ax = {box1.x2 - box1.x1, 0.f, 0.f};
		vec3 ay = {0.f, box1.y2 - box1.y1, 0.f};

		/* Project box on all axis for both boxes */
		/*vec3 min, max;
		tmp.x = box1.x1; tmp.y = box2.y1; /* Bottom left */
		/*min = max = tmp = vec3_project(&tmp, &ax);
		tmp.x = box1.x1; tmp.y = box1.y2; /* Top left */
		/*tmp = vec3_project(&tmp, &ax);
		if (tmp.x > max.x) {
			max.x = tmp.x;
			max.y = tmp.y;
		} else if (tmp.x < min.x) {
			min.x = tmp.x;
			min.y = tmp.y;
		}

		/* Find min and max then compare with other boxes min and max, do they
		   overlap?
		*/

		for (j = 0; j < curr->count; ++j) {
			if (j != i) {
				box2.x1 = curr->object[j].position.x - (1 * 10.f);
				box2.y1 = curr->object[j].position.y - (1 * 10.f);
				box2.x2 = curr->object[j].position.x + (1 * 10.f);
				box2.y2 = curr->object[j].position.y + (1 * 10.f);

				if (box2d_overlap(&box1, &box2)) {
					float_t v1,
							v2;
					
					v1 = vec3_length(&curr->object[i].linear_velocity);
					v2 = vec3_length(&curr->object[j].linear_velocity);
					/* Resolve collision */
					if (v1 > v2) {
						v1 = fabs(box1.x1 - box2.x2);
						v2 = fabs(box1.y1 - box2.y2);
						if (v1 > v2) {
							
						}
					}
				}
			}
		}
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
		game_resolve_collisions(&state_current, &state_previous);
		
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

		if (i > 2) {
			quat a = {10.f, 0.f, 1.f, 0.f};
			a = quat_from_axis(&a);
			curr->object[i].rotation = quat_mul(&curr->object[i].rotation, &a);
		}
	}
}

