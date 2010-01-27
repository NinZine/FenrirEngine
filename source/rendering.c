/*-
 * License
 */

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "rendering.h"

#if defined(__NDS__)
#elif defined(__IPHONE__)
# define glBindFramebuffer glBindFramebufferOES
# define glBindRenderbuffer glBindRenderbufferOES
# define glCheckFramebufferStatus glCheckFramebufferStatusOES
# define glFramebufferRenderbuffer glFramebufferRenderbufferOES
# define glGenFramebuffers glGenFramebuffersOES
# define glGenRenderbuffers glGenRenderbuffersOES
# define glRenderbufferStorage glRenderBufferStorageOES
#else
# define glBindFramebuffer glBindFramebufferEXT
# define glBindRenderbuffer glBindRenderbufferEXT
# define glCheckFramebufferStatus glCheckFramebufferStatusEXT
# define glFramebufferRenderbuffer glFramebufferRenderbufferEXT
# define glGenFramebuffers glGenFramebuffersEXT
# define glGenRenderbuffers glGenRenderbuffersEXT
# define glRenderbufferStorage glRenderbufferStorageEXT
#endif

void
r_bind_buffers(r_state *buffer)
{
	
#if	defined(__NDS__)
#else /* !__NDS__ */
# if defined(__IPHONE__)
	glBindFramebuffer(GL_FRAMEBUFFER_, buffer->framebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer->renderbuffer);
# else /* !__IPHONE__ */
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, buffer->framebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, buffer->renderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
        GL_RENDERBUFFER_EXT, buffer->renderbuffer);
# endif
#endif
}

void
r_bind_depthbuffer(r_state *buffer)
{
	
#if	defined(__NDS__)
#else /* !__NDS__ */
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, buffer->depth);
#endif
}

void
r_clear(float r, float g, float b)
{
	
#if defined(__NDS__)
	glClearColor(31, 31, 31, 31);
	glClearDepth(0x7FFF);
#else /* !__NDS__ */
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
}

void
r_disable_culling()
{

#if defined(__NDS__)
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);
#else /* !__NDS__ */
	glDisable(GL_CULL_FACE);
#endif
}

void
r_enable_culling(GLenum culling)
{

#if defined(__NDS__)
    glPolyFmt(POLY_ALPHA(31) | culling | POLY_FORMAT_LIGHT0);
#else /* !__NDS__ */
	glCullFace(culling);
	glEnable(GL_CULL_FACE);
#endif
}

void
r_enable_light(int8_t light_num)
{
	
#if !defined(__NDS__)
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0 + light_num);
	//glEnable(GL_COLOR_MATERIAL); /* Color is the material */
	glShadeModel(GL_SMOOTH);
#endif
}

uint32_t
r_generate_depthbuffer(int16_t w, int16_t h)
{
	uint32_t d;

#if !defined(__NDS__)
    d = r_generate_renderbuffer();
	
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, d);
	glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT,
		GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, d);
	
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT)
		== GL_FRAMEBUFFER_COMPLETE_EXT);
#endif

    return d;
}

uint32_t
r_generate_framebuffer()
{
    uint32_t r;

#if defined(__NDS__)
#else /* !__NDS__ */
	glGenFramebuffers(1, &r);
#endif

    return r;
}

uint32_t
r_generate_renderbuffer()
{
    uint32_t r;

#if defined(__NDS__)
#else /* !__NDS__ */
	glGenRenderbuffers(1, &r);
#endif

    return r;
}

void
r_present()
{
    
    glFlush();
    glFinish();
    SDL_GL_SwapBuffers();
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
#if defined(__NDS__)
	/* TODO: Use a glCallList or something even faster */
	glBegin(GL_TRIANGLE_STRIP);
	glEnd();
#else /* !__NDS__ */
	glVertexPointer(2, GL_FLOAT, 0, vertex);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glDrawArrays(GL_LINE_LOOP, 0, 63);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 63);
	glDisableClientState(GL_VERTEX_ARRAY);
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
#if defined(__NDS__)
	/* TODO: Implement glCallList or glDrawArrays */
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(quad[0], quad[1], 0);
	glVertex3f(quad[2], quad[3], 0);
	glVertex3f(quad[4], quad[5], 0);
	glVertex3f(quad[6], quad[7], 0);
	glEnd();
#else /* !__NDS__ */
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
	glPopMatrix();
}

void
r_render_ray()
{
	static const GLfloat ray[] = {
		0.0f, 0.f, 0.f,
		1.0f, 0.f, 0.f,
	};
	
#if defined(__NDS__)
#else /* !__NDS__ */
	glPushMatrix();
	glVertexPointer(3, GL_FLOAT, 0, ray);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINE_STRIP, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
#endif
}

/*
 * FIXME: Broken, looks like something round but...
 */
void
r_render_sphere(GLfloat radius)
{
	GLfloat *vertex = malloc(
		((GLuint)(M_PI/.1f + 1) * ((GLuint)(M_2_PI/.1f + 1)))
		* 3 * sizeof(GLfloat));
	GLfloat angle = M_2_PI;
	GLuint i = 0;

	do {
		GLfloat z_angle = M_PI;
		
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
#if defined(__NDS__)
	/* TODO: Definately build a glCallList or implement glDrawArrays */
#else /* !__NDS__ */
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	//glDrawArrays(GL_LINE_LOOP, 0, 2016);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2016);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
	glPopMatrix();
	
	free(vertex);
}

void
r_render_vertices(const GLfloat *vertex, uint8_t n)
{

#if defined(__NDS__)
#else /* !__NDS__ */
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	//glDrawArrays(GL_LINE_LOOP, 0, 2016);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, n);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

void
r_set_clippingarea(int16_t x, int16_t y, int16_t width, int16_t height)
{

#if defined(__NDS__)
#else /* !__NDS__ */
	glScissor(x, y, width, height);
	glEnable(GL_SCISSOR_TEST);
#endif
}

void
r_set_light_position(int light_num, struct vec3 *position)
{

#if defined(__NDS__)
	struct vec3 tmp = vec3_normalize(position);

	light_num = (light_num & 3) << 30;
	GFX_LIGHT_VECTOR = light_num | ((floattov10(tmp.z) & 0x3FF) << 20)
		| ((floattov10(tmp.y) & 0x3FF) << 10)
		| (floattov10(tmp.x) & 0x3FF);
#else /* !__NDS__ */
	GLfloat light_position[] = { position->x, position->y, position->z, 1.0 };

	glLightfv(GL_LIGHT0 + light_num, GL_POSITION, light_position);
#endif
}

void
r_set_material(GLenum type, struct r_color color)
{
	GLfloat tmp[] = { color.red, color.green, color.blue, color.alpha };
	
#if defined(__NDS__)
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
#else /* !__NDS__ */
	if (type == GL_AMBIENT) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, tmp); 
	} else if (type == GL_DIFFUSE) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tmp); 
	} else if (type == GL_EMISSION) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, tmp); 
	} else if (type == GL_SPECULAR) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmp); 
	}
#endif
}

void
r_setup_ambient_light(int8_t light_num, struct r_color color)
{
#if defined(__NDS__)
	rgb tmp = RGB15(((uint8_t)color.red*31), ((uint8_t)color.green*31),
		((uint8_t)color.blue*31));
	
	GFX_LIGHT_COLOR = light_num | tmp;
#else /* !__NDS__ */
	GLfloat light_ambient_color[] = {color.red, color.green, color.blue, color.alpha};
	
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient_color);
	glLightfv(GL_LIGHT0 + light_num, GL_AMBIENT, light_ambient_color);
#endif
}

void
r_setup_diffuse_light(int8_t light_num, struct r_color color)
{
#if defined(__NDS__)
	rgb tmp = RGB15(((uint8_t)color.red*31), ((uint8_t)color.green*31),
		((uint8_t)color.blue*31));

	GFX_LIGHT_COLOR = light_num | tmp;
#else /* !__NDS__ */
	GLfloat light_diffuse_color[] = {color.red, color.green, color.blue, color.alpha};
	
	glLightfv(GL_LIGHT0 + light_num, GL_DIFFUSE, light_diffuse_color);
#endif
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
r_take_screenshot(char *pixels, r_state *buffer)
{

#if defined(__IPHONE__)
	/*glPixelStorei(GL_PACK_ALIGNMENT, 1);*/
	assert(glGetError() == GL_NO_ERROR);
	glReadPixels(0, 0, buffer->width, buffer->height, GL_RGBA,
		GL_UNSIGNED_BYTE, pixels);
#elif defined(__NDS__)
#endif
}

