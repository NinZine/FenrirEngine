/*-
 * License
 */

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "rendering.h"

void
r_bind_buffers(struct r_gl_buffer *buffer)
{
	
#if	__IPHONE__ == 1
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, buffer->frame);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer->render);
#endif
}

void
r_bind_depthbuffer(struct r_gl_buffer *buffer)
{
	
#if	__IPHONE__ == 1
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer->depth);
#endif
}

void
r_clear()
{
	
#if __IPHONE__ == 1
	glClearColor(1.0f, 1.0f, 1.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#elif __NDS__ == 1 /* !__IPHONE__ */
	glClearColor(31, 31, 31, 31);
	glClearDepth(0x7FFF);
#endif /* !__NDS__ */
}

void
r_disable_culling()
{

#if __IPHONE__ == 1
	glDisable(GL_CULL_FACE);
#elif __NDS__ == 1 /* !__IPHONE__*/
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);
#endif /* !__NDS__ */
}

void
r_enable_culling(GLenum culling)
{

#if __IPHONE__ == 1
	glCullFace(culling);
	glEnable(GL_CULL_FACE);
#elif __NDS__ == 1 /* !__IPHONE__ */
    glPolyFmt(POLY_ALPHA(31) | culling | POLY_FORMAT_LIGHT0);
#endif /* !__NDS__ */
}

void
r_enable_light(int8_t light_num)
{
	
#if __IPHONE__ == 1
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0 + light_num);
	//glEnable(GL_COLOR_MATERIAL); /* Color is the material */
	glShadeModel(GL_SMOOTH);
#endif
}

void
r_generate_depthbuffer(struct r_gl_buffer *buffer)
{
	
#if __IPHONE__ == 1
	glGenRenderbuffersOES(1, &buffer->depth);
	
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, buffer->depth);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES,
		buffer->width, buffer->height);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,
		GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, buffer->depth);
	
	assert(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES)
		== GL_FRAMEBUFFER_COMPLETE_OES);
#endif
}

void
r_generate_renderbuffers(struct r_gl_buffer *buffer)
{

#if __IPHONE__ == 1
	glGenFramebuffersOES(1, &buffer->frame);
	glGenRenderbuffersOES(1, &buffer->render);
#endif
}

void
r_render_circle(GLfloat radius)
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
	glRotatef(180.f, 0.f, 1.f, 0.f); /* FIXME: This is a hack */
	glScalef(radius, radius, 1.f);
	glNormal3f(0.0f, 0.0f, 1.0f);
#if __IPHONE__ == 1
	glVertexPointer(2, GL_FLOAT, 0, vertex);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glDrawArrays(GL_LINE_LOOP, 0, 63);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 63);
	glDisableClientState(GL_VERTEX_ARRAY);
#elif __NDS__ == 1
	/* TODO: Use a glCallList or something even faster */
	glBegin(GL_TRIANGLE_STRIP);
	glEnd();
#endif
	glPopMatrix();
	
	//free(vertex);
}

/* TODO: Load a custom matrix instead */
void
r_render_cube(float_t side)
{

	glPushMatrix();
	glScalef(side, side, side);

	/* Front */
	glTranslatef(0.f, 0.f, 0.5f);
	r_render_quad(side);

	/* Back */
	glTranslatef(0.f, 0.f, -1.f);
	glRotatef(180.f, 0.f, 1.f, 0.f);
	r_render_quad(side);

	/* Left */
	glTranslatef(0.5f, 0.f, -0.5f);
	glRotatef(90.f, 0.f, 1.f, 0.f);
	r_render_quad(side);
	
	/* Right */
	glTranslatef(0.f, 0.f, -1.f);
	glRotatef(180.f, 0.f, 1.f, 0.f);
	r_render_quad(side);
	
	/* Bottom */
	glTranslatef(0.f, -.5f, -.5f);
	glRotatef(90.f, 1.f, 0.f, 0.f);
	r_render_quad(side);
	
	/* Top */
	glTranslatef(0.f, 0.f, -1.f);
	glRotatef(180.f, 1.f, 0.f, 0.f);
	r_render_quad(side);
	
	glPopMatrix();
}

void
r_render_quad(float_t side)
{
	static const GLfloat quad[] = {
		-0.5f, -0.5f,
		0.5f,  -0.5f,
		-0.5f,   0.5f,
		0.5f,  0.5f,
	};
	
	glPushMatrix();
	//glScalef(side, side, side);
	glNormal3f(0.0f, 0.0f, 1.0f);
#if __IPHONE__ == 1
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
#elif __NDS__ == 1
	/* TODO: Implement glCallList or glDrawArrays */
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(quad[0], quad[1], 0);
	glVertex3f(quad[2], quad[3], 0);
	glVertex3f(quad[4], quad[5], 0);
	glVertex3f(quad[6], quad[7], 0);
	glEnd();
#endif
	glPopMatrix();
}

/*
 * FIXME: Broken, looks like something round but...
 */
void
r_render_sphere(GLfloat radius)
{
	const GLfloat PI = 3.14;
	const GLfloat PI2 = 6.28;
	GLfloat *vertex = malloc(
		((GLuint)(PI/.1f + 1) * ((GLuint)(PI2/.1f + 1)))
		* 3 * sizeof(GLfloat));
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
#if __IPHONE__ == 1
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	//glDrawArrays(GL_LINE_LOOP, 0, 2016);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2016);
	glDisableClientState(GL_VERTEX_ARRAY);
#elif __NDS__ == 1
	/* TODO: Definately build a glCallList or implement glDrawArrays */
#endif
	glPopMatrix();
	
	free(vertex);
}

void
r_render_vertices(const GLfloat *vertex, uint8_t n)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	//glDrawArrays(GL_LINE_LOOP, 0, 2016);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, n);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void
r_set_clippingarea(int16_t x, int16_t y, int16_t width, int16_t height)
{
	
#if __IPHONE__ == 1
	glScissor(x, y, width, height);
	glEnable(GL_SCISSOR_TEST);
#endif
}

void
r_set_light_position(int light_num, struct vec3 *position)
{
#if __IPHONE__ == 1
	GLfloat light_position[] = { position->x, position->y, position->z, 1.0 };

	glLightfv(GL_LIGHT0 + light_num, GL_POSITION, light_position);
#elif __NDS__ == 1 /* !__IPHONE__ */
	struct vec3 tmp = vec3_normalize(position);

	light_num = (light_num & 3) << 30;
	GFX_LIGHT_VECTOR = light_num | ((floattov10(tmp.z) & 0x3FF) << 20)
		| ((floattov10(tmp.y) & 0x3FF) << 10)
		| (floattov10(tmp.x) & 0x3FF);
#endif /* !__NDS__ */
}

void
r_set_material(GLenum type, struct r_color color)
{
	GLfloat tmp[] = { color.red, color.green, color.blue, color.alpha };
	
#if __IPHONE__ == 1
	if (type == GL_AMBIENT) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, tmp); 
	} else if (type == GL_DIFFUSE) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tmp); 
	} else if (type == GL_EMISSION) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, tmp); 
	} else if (type == GL_SPECULAR) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmp); 
	}
#elif __NDS__ == 1 /* !__IPHONE__ */
	if (type == GL_AMBIENT) {
		glMaterialf(GL_AMBIENT, RGB15( (uint8_t)(tmp[0]*31),
			(uint8_t)(tmp[1]*31), (uint8_t)(tmp[2]*31) ));
	} else if (type == GL_DIFFUSE) {
		glMaterialf(GL_DIFFUSE, RGB15( (uint8_t)(tmp[0]*31),
			(uint8_t)(tmp[1]*31), (uint8_t)(tmp[2]*31) ));
	} else if (type == GL_EMISSION) {
		glMaterialf(GL_EMISSION, RGB15( (uint8_t)(tmp[0]*31),
			(uint8_t)(tmp[1]*31), (uint8_t)(tmp[2]*31) ));
	} else if (type == GL_SPECULAR) {
		glMaterialf(GL_SPECULAR, BIT(15) | RGB15( (uint8_t)(tmp[0]*16),
			(uint8_t)(tmp[1]*16), (uint8_t)(tmp[2]*16) ));
	}
#endif /* !__NDS__ */
}

void
r_setup_ambient_light(int8_t light_num, struct r_color color)
{
#if __IPHONE__ == 1
	GLfloat light_ambient_color[] = { color.red, color.green, color.blue, color.alpha };
	
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient_color);
	glLightfv(GL_LIGHT0 + light_num, GL_AMBIENT, light_ambient_color);
#elif __NDS__ == 1 /* !__IPHONE__ */
	rgb tmp = RGB15(((uint8_t)color.red*31), ((uint8_t)color.green*31),
		((uint8_t)color.blue*31));
	
	GFX_LIGHT_COLOR = light_num | tmp;
#endif /* !__NDS__ */
}

void
r_setup_diffuse_light(int8_t light_num, struct r_color color)
{
#if __IPHONE__ == 1
	GLfloat light_diffuse_color[] = { color.red, color.green, color.blue, color.alpha };
	
	glLightfv(GL_LIGHT0 + light_num, GL_DIFFUSE, light_diffuse_color);
#elif __NDS__ == 1 /* !__IPHONE__ */
	rgb tmp = RGB15(((uint8_t)color.red*31), ((uint8_t)color.green*31),
		((uint8_t)color.blue*31));

	GFX_LIGHT_COLOR = light_num | tmp;
#endif /* !__NDS__ */
}

/*
 * Sets the current viewport with orthogonal
 */
void
r_setup_orthogonal_view(GLfloat width, GLfloat height)
{
	
	glOrthof(-(width/2.0f), (width/2.0f), -(height/2.0f), (height/2.0f),
			-200.0f, 200.0f);
}

/*
 * Sets the current viewport with perspective
 */
void
r_setup_perspective_view(GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far)
{
	GLfloat top = tan(fov*3.14159/360.0) * near;
	GLfloat bottom = -top;
	GLfloat left = aspect * bottom;
	GLfloat right = aspect * top;
	
	glFrustumf(left, right, bottom, top, near, far);
}

/*
 * Takes screenshot. One pixel is 4 bytes
 */
void
r_take_screenshot(char *pixels, struct r_gl_buffer *buffer)
{

#if __IPHONE__ == 1
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	assert(glGetError() == GL_NO_ERROR);
	glReadPixels(0, 0, buffer->width, buffer->height, GL_BGRA,
		GL_UNSIGNED_BYTE, pixels);
#elif __NDS__ == 1
#endif
}

