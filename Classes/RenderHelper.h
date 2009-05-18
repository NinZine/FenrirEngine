/*
 *  RenderHelper.h
 *  Demo2
 *
 *  Created by Andreas Kröhnke on 5/13/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#ifndef _RENDERHELPER_H_
#define _RENDERHELPER_H_

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#include "MathHelper.h"

struct gl_buffer {
	GLuint 	frame;
	GLuint 	render;
	GLuint 	depth;
	GLsizei width;
	GLsizei height;
};

struct color {
	GLfloat alpha;
	GLfloat blue;
	GLfloat green;
	GLfloat red;
};

void bind_buffers(struct gl_buffer *buffer);
void bind_depthbuffer(struct gl_buffer *buffer);

void enable_lighting();

void generate_depthbuffer(struct gl_buffer *buffer);
void generate_renderbuffers(struct gl_buffer *buffer);

void render_circle(GLfloat radius);
void render_sphere(GLfloat radius);
void render_vertices(const GLfloat *vertices, struct color *color);

void setup_ambient_light(Vector position, struct color color);

void setup_orthogonal(GLfloat width, GLfloat height);
void setup_perspective(GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far);

#endif /* !_RENDERHELPER_H_ */
