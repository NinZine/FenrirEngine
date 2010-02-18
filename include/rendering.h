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

#else
# if defined(__APPLE__) /* !__NDS__ */
#  include <OpenGL/OpenGL.h>
# else
#  include "GLee.h"
# endif
# define glFrustumf		glFrustum
# define glOrthof		glOrtho
#endif

#include "vec3.h"

typedef struct r_state {
	uint8_t framebuffer;
	uint8_t renderbuffer;
	uint8_t depth;
	int32_t width;
	int32_t height;
} r_state;


void r_bind_buffers(r_state *state);
void r_bind_depthbuffer(r_state *state);

void r_clear(float r, float g, float b);
void r_color(float r, float g, float b);

uint32_t r_generate_depthbuffer(int16_t w, int16_t h);
uint32_t r_generate_framebuffer();
uint32_t r_generate_renderbuffer();

void r_disable_culling();

void r_enable_culling(GLenum culling);
void r_enable_light(int8_t n);

void r_load_identity();
void r_load_matrix(const float m[4][4]);

void r_pop_matrix();
void r_present();
void r_push_matrix();

void r_render_circle(float radius);
void r_render_cube(float side);
void r_render_line(float x1, float y1, float z1, float x2, float y2, float z2);
void r_render_quad(float side);
void r_render_ray();
void r_render_sphere(float radius);
void r_render_vertices(const float *vertex, uint16_t n);
void r_rotate(float degrees, float x, float y, float z);

void r_scale(float sx, float sy, float sz);
void r_set_clippingarea(int16_t x, int16_t y, int16_t width, int16_t height);
void r_set_light_position(int n, vec3 *position);
void r_set_material(GLenum type, float r, float g, float b);

void r_setup_ambient_light(int8_t n, float r, float g, float b);
void r_setup_diffuse_light(int8_t n, float r, float g, float b);
void r_setup_orthogonal_view(float width, float height);
void r_setup_perspective_view(float fov, float aspect, float n, float f);

void r_take_screenshot(char *pixels, r_state *buffer);
void r_translate(const vec3 *v);

#endif /* !_RENDERING_H_ */

