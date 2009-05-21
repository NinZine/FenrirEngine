/*
 *  RenderHelper.h
 *  Demo2
 *
 *  Created by Andreas Kröhnke on 5/13/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#ifndef _RENDERING_H_
#define _RENDERING_H_

#include <sys/types.h>

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#include "vec3.h"

struct r_gl_buffer {
	GLuint 	frame;
	GLuint 	render;
	GLuint 	depth;
	GLsizei width;
	GLsizei height;
};

struct r_color {
	GLfloat alpha;
	GLfloat blue;
	GLfloat green;
	GLfloat red;
};

void r_bind_buffers(struct r_gl_buffer *buffer);
void r_bind_depthbuffer(struct r_gl_buffer *buffer);

void r_generate_depthbuffer(struct r_gl_buffer *buffer);
void r_generate_renderbuffers(struct r_gl_buffer *buffer);

void r_enable_light(int8_t n);

void r_render_circle(GLfloat radius);
void r_render_cube(float_t side);
void r_render_quad(float_t side);
void r_render_sphere(GLfloat radius);
void r_render_vertices(const GLfloat *vertices, struct r_color *color);

void r_set_clippingarea(int16_t x, int16_t y, int16_t width, int16_t height);
void r_set_light_position(int8_t n, vec3 *position);

void r_setup_ambient_light(int8_t n, struct r_color color);
void r_setup_diffuse_light(int8_t n, struct r_color color);
void r_setup_orthogonal_view(GLfloat width, GLfloat height);
void r_setup_perspective_view(GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far);

void r_take_screenshot(char *pixels, struct r_gl_buffer *buffer);

#endif /* !_RENDERING_H_ */

