/*-
 * License
 */

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(__SDL__)
# include <SDL/SDL.h>
#endif

#include "rendering.h"

#if defined(__NDS__)
#elif defined(__IPHONE__)
# define GL_DEPTH_ATTACHMENT_EXT GL_DEPTH_ATTACHMENT_OES
# define GL_DEPTH_COMPONENT16 GL_DEPTH_COMPONENT16_OES
# define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE_OES
# define GL_FRAMEBUFFER_EXT GL_FRAMEBUFFER_OES
# define GL_RENDERBUFFER_EXT GL_RENDERBUFFER_OES
# define GL_RGBA32F_ARB GL_RGBA

# define glBindFramebuffer glBindFramebufferOES
# define glBindRenderbuffer glBindRenderbufferOES
# define glCheckFramebufferStatus glCheckFramebufferStatusOES
# define glFramebufferRenderbuffer glFramebufferRenderbufferOES
# define glGenFramebuffers glGenFramebuffersOES
# define glGenRenderbuffers glGenRenderbuffersOES
# define glRenderbufferStorage glRenderbufferStorageOES
#elif defined(__APPLE__)
# define glBindFramebuffer glBindFramebufferEXT
# define glBindRenderbuffer glBindRenderbufferEXT
# define glCheckFramebufferStatus glCheckFramebufferStatusEXT
# define glFramebufferRenderbuffer glFramebufferRenderbufferEXT
# define glGenFramebuffers glGenFramebuffersEXT
# define glGenRenderbuffers glGenRenderbuffersEXT
# define glRenderbufferStorage glRenderbufferStorageEXT
#endif

#if defined(__SDL__)
static SDL_Surface *surface;
#endif

void
r_bind_buffers(r_state *buffer)
{
	
#if	defined(__NDS__)
#elif defined(__IPHONE__) /* !__NDS__ */
	glBindFramebuffer(GL_FRAMEBUFFER_OES, (GLuint)buffer->framebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER_OES, (GLuint)buffer->renderbuffer);
#else /* !__IPHONE__ */
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, buffer->framebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, buffer->renderbuffer);
    /*glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
        GL_RENDERBUFFER_EXT, buffer->renderbuffer);
    */
#endif
}

void
r_bind_depthbuffer(r_state *buffer)
{
	
#if	defined(__NDS__)
#elif defined(__IPHONE__) /* !__NDS__ */
	glBindRenderbuffer(GL_RENDERBUFFER_OES, buffer->depth);
#else
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, buffer->depth);
#endif
}

void
r_bind_texture(uint16_t id)
{
    
    glBindTexture(GL_TEXTURE_2D, id);
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
r_color(float r, float g, float b)
{
	
	glColor3f(r, g, b);
}

void r_color4(float r, float g, float b, float a)
{
	
	glColor4f(r, g, b, a);
}

void
r_create_window(uint16_t w, uint16_t h)
{
#if defined(__SDL__)
	const SDL_VideoInfo *info;
	int flags = 0;
	uint8_t depth = 0;

	info = SDL_GetVideoInfo();
	depth = info->vfmt->BitsPerPixel;
	flags = SDL_OPENGL;

	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1))
	    printf("sdl> no double buffer\n");
	if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8))
	    printf("sdl> no depth buffer\n");
	if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8))
	    printf("sdl> no stencil buffer\n");
	if (SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1))
	    printf("sdl> no swapping\n");

	surface = SDL_SetVideoMode(w, h, depth, flags);
	if (!surface) {
	    printf("sdl> no video\n");
	    exit(1);
	}
#endif
}

void
r_disable_blending()
{

	glDisable(GL_BLEND);
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
r_enable_blending()
{

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	/*
    glBindRenderbuffer(GL_RENDERBUFFER_EXT, r);
	glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_RGBA32F_ARB, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);*/
#endif

    return r;
}

float*
r_generate_texcoords(uint32_t width, uint32_t heigth,
	uint32_t tile_w, uint32_t tile_h)
{
	uint32_t columns, rows, x, y, i;
	float *coord, size_x, size_y;

	columns = width / tile_w;
	rows = heigth / tile_h;
	size_x = 1.f / columns;
	size_y = 1.f / rows;
	coord = malloc(columns * rows * 4 * 2 * sizeof(float));

	for (y = 0; y < rows*8; y+=8) {
		for (x = 0; x < columns*8; x+=8) {
			i = y * columns + x;
			coord[i + 0] = x/8 * size_x;
			coord[i + 1] = 1.f - y/8 * size_y - size_y;
			coord[i + 2] = coord[i + 0] + size_x;
			coord[i + 3] = coord[i + 1];
			coord[i + 4] = coord[i + 0];
			coord[i + 5] = coord[i + 1] + size_y;
			coord[i + 6] = coord[i + 0] + size_x;
			coord[i + 7] = coord[i + 1] + size_y;
		}
	}

	return coord;
}

void
r_load_identity()
{
    glLoadIdentity();
}

void
r_load_matrix(const float m[4][4])
{
	glLoadMatrixf((GLfloat *)m);
}

void
r_look_at(float eye_x, float eye_y, float eye_z,
	float center_x, float center_y, float center_z,
	float up_x, float up_y, float up_z)
{
	vec3 f, s, u; /* forward, side, up */
	float m[4][4];

	f.x = center_x - eye_x; f.y = center_y - eye_y; f.z = center_z - eye_z;
	f = vec3_normalize(&f);
	u.x = up_x; u.y = up_y; u.z = up_z;
	u = vec3_normalize(&u);
	s = vec3_cross(&f, &u);
	u = vec3_cross(&s, &f);

	m[0][0] = s.x;	m[1][0] = s.y;	m[2][0] = s.z; m[3][0] = 0.f;
	m[0][1] = u.x;	m[1][1] = u.y;	m[2][1] = u.z; m[3][1] = 0.f;
	m[0][2] = -f.x;	m[1][2] = -f.y;	m[2][2] = -f.z; m[3][2] = 0.f;
	m[3][3] = 1.f;

	glMultMatrixf((GLfloat *)m);
	glTranslatef(-eye_x, -eye_y, -eye_z);
}

void
r_pop_matrix()
{
    glPopMatrix();
}

void
r_present()
{
    
    glFlush();
#if defined(__SDL__)
    SDL_GL_SwapBuffers();
#endif
}

void
r_push_matrix()
{
    glPushMatrix();
}

void
r_quit()
{
	
#if defined(__SDL__)
	SDL_FreeSurface(surface);
	surface = 0;
#endif
}

void
r_render_circle(float radius)
{
	static GLfloat *vertex = NULL;
	GLfloat angle = 6.28f;
	
	if (vertex == NULL) {
		vertex = (GLfloat*)malloc((GLuint)(angle/.1f + 1) * 2 * sizeof(GLfloat));
		uint16_t i = 0;
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
r_render_cube(float side)
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
r_render_line(float x1, float y1, float z1, float x2, float y2, float z2)
{
	static GLfloat ray[] = {
		0.0f, 0.f, 0.f,
		1.0f, 0.f, 0.f,
	};

    ray[0] = x1; ray[1] = y1; ray[2] = z1;
    ray[3] = x2; ray[4] = y2; ray[5] = z2;
	
#if defined(__NDS__)
#else /* !__NDS__ */
	glVertexPointer(3, GL_FLOAT, 0, ray);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINE_STRIP, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

void
r_render_mesh(mesh *m)
{
	r_render_triangles(m->vertex, m->vertices, m->face, m->faces);
}

void
r_render_quad(float side)
{
	static const GLfloat quad[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		-0.5f,  0.5f,
		 0.5f,  0.5f,
	};
	static const GLfloat coords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
	};
	
	glPushMatrix();
	glScalef(side, side, side);
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
    glEnable(GL_TEXTURE_2D);
	glTexCoordPointer(2, GL_FLOAT, 0, coords);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
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
r_render_sphere(float radius)
{
	GLfloat *vertex = (GLfloat*)malloc(
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
r_render_texquad(float side, float *texcoords, uint32_t offset)
{
	static const GLfloat quad[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		-0.5f,  0.5f,
		 0.5f,  0.5f,
	};
	static const GLfloat coords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
	};
	
	glPushMatrix();
	glScalef(side, side, side);
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
    glEnable(GL_TEXTURE_2D);
	if (texcoords == NULL) {
		glTexCoordPointer(2, GL_FLOAT, 0, coords);
	} else {
		glTexCoordPointer(2, GL_FLOAT, 0, texcoords+8*offset);
	}
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
#endif
	glPopMatrix();
}

void
r_render_triangles(float *vertex, uint16_t vertices, uint16_t *face,
	uint16_t faces)
{
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawElements(GL_TRIANGLES, 3*faces, GL_UNSIGNED_SHORT, face);
	glDisableClientState(GL_VERTEX_ARRAY);

#if defined(DEBUG)
	int i;

	glColor3f(1, 0, 0);
	glBegin(GL_POINTS); 
	for(i=0; i<3*faces; ++i)  {
		glVertex3f(vertex[face[i]*3], vertex[face[i]*3+1], vertex[face[i]*3+2]); 
	}
	glEnd();
#endif
	
}

void
r_render_vertices(const float *vertex, uint16_t n)
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
r_rotate(float degrees, float x, float y, float z)
{

    glRotatef(degrees, x, y, z);
}

void
r_scale(float sx, float sy, float sz)
{
    
    glScalef(sx, sy, sz);
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
r_set_material(GLenum type, float r, float g, float b)
{
	
#if defined(__NDS__)
	if (type == GL_AMBIENT) {
		glMaterialf(GL_AMBIENT, RGB15( (uint8_t)(r*31),
			(uint8_t)(g*31), (uint8_t)(b*31) ));
	} else if (type == GL_DIFFUSE) {
		glMaterialf(GL_DIFFUSE, RGB15( (uint8_t)(r*31),
			(uint8_t)(g*31), (uint8_t)(b*31) ));
	} else if (type == GL_EMISSION) {
		glMaterialf(GL_EMISSION, RGB15( (uint8_t)(r*31),
			(uint8_t)(g*31), (uint8_t)(b*31) ));
	} else if (type == GL_SPECULAR) {
		glMaterialf(GL_SPECULAR, BIT(15) | RGB15( (uint8_t)(r*16),
			(uint8_t)(g*16), (uint8_t)(b*16) ));
	}
#else /* !__NDS__ */
    GLfloat t[4] = {r, g, b, 1.0f};

	if (type == GL_AMBIENT) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, t);
	} else if (type == GL_DIFFUSE) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, t);
	} else if (type == GL_EMISSION) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, t);
	} else if (type == GL_SPECULAR) {
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, t); 
	}
#endif
}

void
r_setup_ambient_light(int8_t light_num, float r, float g, float b)
{
#if defined(__NDS__)
	rgb tmp = RGB15(((uint8_t)r*31), ((uint8_t)g*31),
		((uint8_t)b*31));
	
	GFX_LIGHT_COLOR = light_num | tmp;
#else /* !__NDS__ */
	GLfloat t[4] = {r, g, b, 1.0f};

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient_color);
	glLightfv(GL_LIGHT0 + light_num, GL_AMBIENT, t);
#endif
}

void
r_setup_diffuse_light(int8_t light_num, float r, float g, float b)
{
#if defined(__NDS__)
	rgb tmp = RGB15(((uint8_t)r*31), ((uint8_t)g*31),
		((uint8_t)b*31));

	GFX_LIGHT_COLOR = light_num | tmp;
#else /* !__NDS__ */
	GLfloat t[4] = {r, g, b, 1.0f};

	glLightfv(GL_LIGHT0 + light_num, GL_DIFFUSE, t);
#endif
}

void
r_setup_orthogonal_view(float width, float height)
{

	glMatrixMode(GL_PROJECTION);
	glOrthof(-(width/2.0f), (width/2.0f), -(height/2.0f), (height/2.0f),
			-200.0f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
}

void
r_setup_perspective_view(float fov, float aspect, float n, float f)
{
	GLfloat top = tan(fov*M_PI/360.0) * n;
	GLfloat bottom = -top;
	GLfloat left = aspect * bottom;
	GLfloat right = aspect * top;
	
	glMatrixMode(GL_PROJECTION);
	glFrustumf(left, right, bottom, top, n, f);
	glMatrixMode(GL_MODELVIEW);
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

void
r_translate(float x, float y, float z)
{
	
	glTranslatef(x, y, z);
}

uint16_t
r_upload_texture(uint32_t w, uint32_t h, void *image_data)
{
    GLuint id;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
        (GLvoid*)image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return id;
}

