/*-
	Copyright (C) 2009 Andreas Kroehnke <ninzine@indraz.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
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

#include "array.h"
#include "behavior.h"
#include "box2d.h"
#include "game.h"
#include "game_entity.h"
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

static uint32_t camera_id;
static vec3 light_position = {0.0f, -1.0f, 0.0f};
static g_entities entities = {};
static struct gh_state state_current = {0,0};
static struct gh_state state_previous = {0,0};
static gh_time game_time = {0, 0, 0, 0, 0, 1.f / 20.f};

static int gh_index_of_entity(uint32_t id);
static void game_initialize_light();
static void game_input_handle();
static void game_interpolate_states(struct gh_state *out,
	struct gh_state *curr, struct gh_state *prev, float_t t);
static void game_render_state(const g_entity *g, uint8_t n,
	struct gh_state *src);
static void game_resolve_collisions(struct gh_state *curr,
	struct gh_state *prev);
static void game_update_state(struct gh_state *curr);
static void game_update_entities(g_entities *e);

g_entity*
gh_create_entity()
{
	static int id = 0;
	g_entities *state = &entities;
	
	gh_array_resize((void**)&state->object, state->count,
					sizeof(g_entity), 1);
	
	bzero(&state->object[state->count], sizeof(g_entity));
	state->object[state->count].id = ++id;
	++state->count;
	return &state->object[state->count-1];
}

/* FIXME: Something weird happens with to many entities. */
uint32_t
gh_create_rigidbody(vec3 *position, quat *rotation,
					vec3 *scale, vec3 *vel, vec3 *ang_vel)
{
	static uint32_t id = 0;
	struct gh_state *state = &state_current;
	
	gh_array_resize((void**)&state->object, state->count,
					sizeof(struct gh_rigid_body), 1);
	
	bzero(&state->object[state->count], sizeof(gh_rigid_body));
	state->object[state->count].id = ++id;
	if (0 != position) {
		state->object[state->count].position = *position;
	}
	if (0 != rotation) {
		state->object[state->count].rotation = quat_from_axis(rotation);
	}
	if (0 != scale) {
		state->object[state->count].scale = *scale;
	}
	if (0 != vel) {
		state->object[state->count].linear_velocity = *vel;
	}
	if (0 != ang_vel) {
		state->object[state->count].angular_velocity = *ang_vel;
	}
	
	++state->count;
	return state->object[state->count-1].id;
}

void
gh_delete_entity(uint32_t id)
{
	int i;
	g_entity tmp;
	
	/* Locate it */
	i = gh_index_of_entity(id);
	if (-1 == i) {
		return;
	}
	/* Place it last in the array */
	tmp = entities.object[i];
	entities.object[i] = entities.object[entities.count-1];
	/* Clean it */
	for (i = 0; i < tmp.behaviors; ++i) {
		b_clean_behavior(&tmp.b[i]);
	}
	for (i = 0; i < tmp.models; ++i) {
		if (S_POLYGON == tmp.m[i].shape) {
			if (0 != tmp.m[i].edges) free(tmp.m[i].edge);
			if (0 != tmp.m[i].vertices) free(tmp.m[i].vertex);
		}
	}
	if (0 != tmp.b) free(tmp.b);
	if (0 != tmp.m) free(tmp.m);
	
	/* Resize the array */
	gh_array_resize((void**)&entities.object, entities.count,
		sizeof(g_entity), -1);
	/* TODO: Sort the array */
}

int
gh_index_of_entity(uint32_t id)
{
	int i, index;
	bool found = false;
	
	for (i = 0; !found && i < entities.count; ++i) {
		if (entities.object[i].id == id) {
			index = i;
			found = true;
		}
	}
	
	return index;
}

g_entity*
game_get_entity(uint32_t id)
{
	int i;
	
	i = gh_index_of_entity(id);
	if (-1 == i) {
		return 0;
	} else {
		return &entities.object[i];
	}
}

struct gh_rigid_body*
game_get_rigidbody(uint32_t id)
{
	int i;
	bool found = false;
	struct gh_rigid_body *tmp = 0;
	
	for (i = 0; !found && i < state_current.count; ++i) {
		if (state_current.object[i].id == id) {
			tmp = &state_current.object[i];
			found = true;
		}
	}
	
	return tmp;
}

void
game_initialize()
{
	int i;

	if (0 == entities.count) {
		
		for (i = 0; i < 5; ++i) {
			g_entity *e;
			vec3 position = {
				0.f,
				(5 * -20.f) + (i * 40.f),
				0.f};
			quat rotation = {10.f * i, 0.f, 0.f, 1.f};
			vec3 scale = {20.f, 20.f, 20.f};
			vec3 vel = {0.f, 0.f, 0.f};
			vec3 ang_vec = {0.f, 0.f, (i > 2) ? 10.f : 0.f};
			vec3 edge[2] = {
				{1.f, 0.f, 0.f},
				{0.f, 1.f, 0.f},
			}; /* TODO: Find edges, this should be done at load. */
			vec3 point[4] = {
				{-0.5f, -0.5f, 0.f},
				{ 0.5f, -0.5f, 0.f},
				{-0.5f,  0.5f, 0.f},
				{ 0.5f,  0.5f, 0.f},
			}; /* TODO: This is vertex data */
			
			e = gh_create_entity();
			gh_array_resize((void**)&e->m, 0, sizeof(struct gh_model), 1);
			e->models = 1;
			e->m->vertices = 4;
			e->m->edges = 2;
			e->m->shape = S_POLYGON;
			gh_array_resize((void**)&e->m->vertex, 0, sizeof(vec3), 4);
			gh_array_resize((void**)&e->m->edge, 0, sizeof(vec3), 2);
			memcpy(e->m->vertex, point, 4 * sizeof(vec3));
			memcpy(e->m->edge, edge, 2 * sizeof(vec3));
			
			e->rb = gh_create_rigidbody(&position,
					&rotation, &scale, &vel, &ang_vec);
		}
		
		/* Camera is also a rigid body */
		camera_id = gh_create_rigidbody(0, 0, 0, 0, 0);
	}
	
	gh_copy_state(&state_previous, &state_current, true);
	
	game_initialize_light();
	/* Test behavior */
	b_add_behavior(&entities.object[2].b, &entities.object[2].behaviors);
	b_add_rule(entities.object[2].b, "see");
	b_set_attribute(entities.object[2].b->attr, entities.object[2].b->attrs,
		"distance", 100.f);
	b_set_attribute(entities.object[2].b->attr, entities.object[2].b->attrs,
		"you", entities.object[1].id);
	b_add_action(entities.object[2].b, "move");
	b_set_attribute(entities.object[2].b->attr, entities.object[2].b->attrs,
		"speed", 2.f);
	
	b_add_behavior(&entities.object[1].b, &entities.object[1].behaviors);
	b_add_rule(entities.object[1].b, "input");
	b_add_action(entities.object[1].b, "move");
	b_set_attribute(entities.object[1].b->attr, entities.object[1].b->attrs,
		"speed", 4.f);
	/* Right button */
	b_add_behavior(&entities.object[1].b, &entities.object[1].behaviors);
	b_add_rule(&entities.object[1].b[1], "input");
	b_set_attribute(entities.object[1].b[1].attr, entities.object[1].b[1].attrs, "button", 1);
	b_add_action(&entities.object[1].b[1], "shoot");
	
	/* End test */
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
game_interpolate_states(struct gh_state *out, struct gh_state *curr,
		struct gh_state *prev, float_t t)
{
	int i;

	if (out->count != curr->count || out->count != prev->count) {
		//warn("gh_state is not the same size");
	}

	for (i = 0; i < prev->count; ++i) {
		
		out->object[i].position = vec3_lerp(&prev->object[i].position,
			&curr->object[i].position, t);
		out->object[i].rotation = quat_slerp(&prev->object[i].rotation,
			&curr->object[i].rotation, t);
	}
}

void
game_render(struct r_gl_buffer *buffer)
{
	static struct gh_state tmp = {};
	static float_t aspect_ratio = 0.f;
	float_t interpolate = /*1.0f - */game_time.accumulator/game_time.timestep;

	/* First frame */
	if (0 == tmp.count) {
		//gh_copy_state(&tmp, &state_current, false);

		aspect_ratio = (float_t)buffer->width / (float_t)buffer->height;
	}
	gh_copy_state(&tmp, &state_current, false);
	game_interpolate_states(&tmp, &state_current, &state_previous,
							interpolate);

	/* Setup buffers and view */
	r_bind_buffers(buffer);
	glViewport(0, 0, buffer->width, buffer->height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	r_setup_orthogonal_view(buffer->width, buffer->height);
	//r_setup_perspective_view(60.0f, aspect_ratio, 0.01f, 1000.f);
	//camera_tmp = vec3_lerp(&camera_previous, &camera_current, interpolate);
	//glTranslatef(-camera_tmp.x, -camera_tmp.y, -camera_tmp.z);
	glRotatef(90.0f, 0.0f, 0.0f, -1.0f); // For landscape mode
	glTranslatef(-tmp.object[5].position.x, -tmp.object[5].position.y,
				 -tmp.object[5].position.z);
	
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
	
	game_render_state(entities.object, entities.count, &tmp);
	
	/* Make sure there is no error in OpenGL stuff */
	assert(glGetError() == GL_NO_ERROR);
}

void
game_render_state(const g_entity *g, const uint8_t n, struct gh_state *src)
{
	static const struct r_color ambient = {1.f, 0.7f, 0.7f, 0.7f};
	static const struct r_color diffuse = {1.f, 0.8f, 0.5f, 0.1f};
	int i;

	for (i = 0; i < n; ++i) {
		mat4 tf;

		if (0 == g[i].rb || 0 == g[i].models) {
			continue;
		}
		
		glColor4f(0.9, 0.6, 0.4, 1.f);
		glPushMatrix();
		/* Only effective without glEnable(GL_COLOR_MATERIAL) */
		r_set_material(GL_AMBIENT, ambient);
		r_set_material(GL_DIFFUSE, diffuse);
		
		glEnable(GL_COLOR_MATERIAL);
		gh_build_mat4(&src->object[i], &tf);
		glLoadMatrixf((GLfloat *)tf.m);
		if (S_CIRCLE == g[i].m->shape)
			r_render_circle(1);
		else if (S_POLYGON == g[i].m->shape)
			r_render_vertices((GLfloat *)g[i].m->vertex, g[i].m->vertices);
		else if (S_QUAD == g[i].m->shape)
			r_render_quad(1);
		else if (S_RAY == g[i].m->shape)
			r_render_ray();
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
		
		gh_copy_state(&state_previous, &state_current, false);
		game_update_entities(&entities);
		game_update_state(&state_current);
		
		game_time.accumulator -= game_time.timestep;
		game_time.frame += 1;
	}
}

void
game_update_entities(g_entities *e)
{
	int i, j;
	
	for (i = 0; i < e->count; ++i) {
		if (0 != e->object[i].behaviors) {
			for (j = 0; j < e->object[i].behaviors; ++j) {
				b_exec((void*)&e->object[i], &e->object[i].b[j]);
			}
		}
	}
}

void
game_update_state(struct gh_state *curr)
{
	int16_t i;
	gh_rigid_body	*camera,
					*player;
	
	camera = game_get_rigidbody(camera_id);
	player = game_get_rigidbody(entities.object[1].rb);
	camera->position.x = player->position.x;
	camera->position.y = player->position.y;
	//camera_current.z = entity[1].rb->position.z;

	for (i = 0; i < curr->count; ++i) {
		quat a = {1.f,
			curr->object[i].angular_velocity.x,
			curr->object[i].angular_velocity.y,
			curr->object[i].angular_velocity.z
		};
		
		curr->object[i].linear_velocity =
			vec3_mul(&curr->object[i].linear_velocity, 0.9f);
		curr->object[i].position = vec3_add(&curr->object[i].position,
				&curr->object[i].linear_velocity);

		a = quat_from_axis(&a);
		curr->object[i].rotation = quat_mul(&curr->object[i].rotation, &a);
	}
}

