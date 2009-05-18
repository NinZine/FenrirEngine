/*
 *  RenderHelper.c
 *  Demo2
 *
 *  Created by Andreas Kröhnke on 5/13/09.
 *  Copyright 2009 BTH. All rights reserved.
 *
 */

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "RenderHelper.h"

void
bind_buffers(struct gl_buffer *buffer)
{
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, buffer->frame);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer->render);
}

void
bind_depthbuffer(struct gl_buffer *buffer)
{
	
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer->depth);
}

void
enable_lighting()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_FLAT);
}

void
generate_depthbuffer(struct gl_buffer *buffer)
{
	
	glGenRenderbuffersOES(1, &buffer->depth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &buffer->width);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &buffer->height);
	
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer->depth);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, buffer->width, buffer->height);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES,
								 GL_RENDERBUFFER_OES, buffer->depth);
	
	assert(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) == GL_FRAMEBUFFER_COMPLETE_OES);
}

void
generate_renderbuffers(struct gl_buffer *buffer)
{

	glGenFramebuffersOES(1, &buffer->frame);
	glGenRenderbuffersOES(1, &buffer->render);
}

void
render_circle(GLfloat radius)
{
	static GLfloat *vertex = NULL;
	GLfloat angle = 6.28f;
	
	if (vertex == NULL) {
		vertex = malloc((GLuint)(angle/.1f + 1) * 2 * sizeof(GLfloat));
		GLuint i = 0;
		do {
			GLfloat x, y;
			x = /*point.x + */cos(angle);
			y = /*point.y + */sin(angle);
			
			vertex[i] = x;
			vertex[i+1] = y;
			
			i += 2;
			angle = angle - 0.1f;
		} while (angle > 0.f);
	}
	
	glPushMatrix();
	glScalef(radius, radius, 1.f);
	glVertexPointer(2, GL_FLOAT, 0, vertex);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glDrawArrays(GL_LINE_LOOP, 0, 63);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 63);
	glPopMatrix();
	
	//free(vertex);
}

void
render_sphere(GLfloat radius)
{
	const GLfloat PI = 3.14;
	const GLfloat PI2 = 6.28;
	GLfloat *vertex = malloc( ((GLuint)(PI/.1f + 1) * ((GLuint)(PI2/.1f + 1))) * 3 * sizeof(GLfloat));
	GLfloat angle = PI2;
	GLuint i = 0;

	do {
		GLfloat z_angle = PI;
		
		do {
			GLfloat x, y, z;
			
			x = cos(angle) * sin(z_angle);
			y = sin(angle) * sin(z_angle);
			z = cos(z_angle);
			
			vertex[i] = x;
			vertex[i+1] = y;
			vertex[i+2] = z;
			
			z_angle = z_angle - .1f;
			i += 3;
		} while (z_angle > 0.f);
		
		angle = angle - .1f;
	} while (angle > 0.f);
	
	glPushMatrix();
	glColor4f(0.f, 0.f, 0.f, 1.f);
	glScalef(radius, radius, radius);
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glDrawArrays(GL_LINE_LOOP, 0, 2016);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2016);
	glPopMatrix();
	
	free(vertex);
}

void
setup_ambient_light(Vector position, struct color color)
{
	GLfloat lightAmbientColor[] = { color.red, color.green, color.blue, color.alpha };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightAmbientColor);
	
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01f);
	
	GLfloat lightPosition[] = { position.x, position.y, position.z, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

/*
 * Sets the current viewport with orthogonal
 */
void
setup_orthogonal(GLfloat width, GLfloat height)
{
	
	glOrthof(-(width/2.0f), (width/2.0f), -(height/2.0f), (height/2.0f), -200.0f, 200.0f);
}

/*
 * Sets the current viewport with perspective
 */
void
setup_perspective(GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far)
{
	GLfloat top = tan(fov*3.14159/360.0) * near;
	GLfloat bottom = -top;
	GLfloat left = aspect * bottom;
	GLfloat right = aspect * top;
	
	glFrustumf(left, right, bottom, top, near, far);
}

