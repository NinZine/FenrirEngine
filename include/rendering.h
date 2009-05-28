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

#include <math.h>

#if defined(__IPHONE__)
# define __NDS__ 0
# include <OpenGLES/ES1/gl.h>
# include <OpenGLES/ES1/glext.h>
#elif defined(__NDS__) /* !__IPHONE__ */
# define __IPHONE__ 0
# include <nds.h>
	typedef uint8_t			GLenum;
	typedef float           GLfloat;
	typedef uint8_t         GLuint;
	typedef int             GLsizei;

# define GL_FRONT				POLY_CULL_FRONT
# define GL_BACK				POLY_CULL_BACK
# define GL_FRONT_AND_BACK		GL_FRONT | GL_BACK
# define glColor4f(r, g, b, a)  glColor3f(r, g, b)
# define glFrustumf				glFrustum
# define glOrthof				glOrtho
# define glPopMatrix() 			glPopMatrix(1)

#endif /* !__NDS__ */

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

void r_clear();

void r_generate_depthbuffer(struct r_gl_buffer *buffer);
void r_generate_renderbuffers(struct r_gl_buffer *buffer);

void r_disable_culling();

void r_enable_culling(GLenum culling);
void r_enable_light(int8_t n);

void r_render_circle(GLfloat radius);
void r_render_cube(float_t side);
void r_render_quad(float_t side);
void r_render_sphere(GLfloat radius);
void r_render_vertices(const GLfloat *vertices, struct r_color *color);

void r_set_clippingarea(int16_t x, int16_t y, int16_t width, int16_t height);
void r_set_light_position(int n, vec3 *position);
void r_set_material(GLenum type, struct r_color color);

void r_setup_ambient_light(int8_t n, struct r_color color);
void r_setup_diffuse_light(int8_t n, struct r_color color);
void r_setup_orthogonal_view(GLfloat width, GLfloat height);
void r_setup_perspective_view(GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far);

void r_take_screenshot(char *pixels, struct r_gl_buffer *buffer);

#endif /* !_RENDERING_H_ */

