/*-
 * License
 */

#ifndef _RENDERING_H_
#define _RENDERING_H_

#include <math.h>
#include <stdint.h>

#if defined(__IPHONE__)
# include <OpenGLES/ES1/gl.h>
# include <OpenGLES/ES1/glext.h>
#elif defined(__NDS__) /* !__IPHONE__ */
# include <nds.h>
# define GL_FRONT				POLY_CULL_FRONT
# define GL_BACK				POLY_CULL_BACK
# define GL_FRONT_AND_BACK		GL_FRONT | GL_BACK
# define glColor4f(r, g, b, a)  glColor3f(r, g, b)
# define glFrustumf				glFrustum
# define glOrthof				glOrtho
# define glPopMatrix() 			glPopMatrix(1)

#elif defined(__APPLE__) /* !__NDS__ */
# include <OpenGL/OpenGL.h>
# define glFrustumf		glFrustum
# define glOrthof		glOrtho
#endif /* !__APPLE__ */

#include "vec3.h"

typedef struct r_state {
	uint8_t framebuffer;
	uint8_t renderbuffer;
	uint8_t depth;
	int32_t width;
	int32_t height;
} r_state;

struct r_color {
	GLfloat alpha;
	GLfloat blue;
	GLfloat green;
	GLfloat red;
};

void r_bind_buffers(r_state *state);
void r_bind_depthbuffer(r_state *state);

void r_clear(float r, float g, float b);

uint32_t r_generate_depthbuffer(int16_t w, int16_t h);
uint32_t r_generate_framebuffer();
uint32_t r_generate_renderbuffer();

void r_disable_culling();

void r_enable_culling(GLenum culling);
void r_enable_light(int8_t n);

void r_present();

void r_render_circle(float radius);
void r_render_cube(float side);
void r_render_quad(float side);
void r_render_ray();
void r_render_sphere(float radius);
void r_render_vertices(const float *vertices, uint8_t n);

void r_set_clippingarea(int16_t x, int16_t y, int16_t width, int16_t height);
void r_set_light_position(int n, vec3 *position);
void r_set_material(GLenum type, struct r_color color);

void r_setup_ambient_light(int8_t n, struct r_color color);
void r_setup_diffuse_light(int8_t n, struct r_color color);
void r_setup_orthogonal_view(float width, float height);
void r_setup_perspective_view(float fov, float aspect, float near, float far);

void r_take_screenshot(char *pixels, r_state *buffer);

#endif /* !_RENDERING_H_ */

