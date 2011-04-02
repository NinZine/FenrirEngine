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
#elif defined(__ANDROID__) /* !__IPHONE__ */
# include <GLES/gl.h>
# include <GLES/glext.h>
#elif defined(__NDS__) /* !__ANDROID__ */
# include <nds.h>
# define GL_FRONT				POLY_CULL_FRONT
# define GL_BACK				POLY_CULL_BACK
# define GL_FRONT_AND_BACK		GL_FRONT | GL_BACK
# define glColor4f(r, g, b, a)  glColor3f(r, g, b)
# define glFrustumf				glFrustum
# define glOrthof				glOrtho
# define glPopMatrix() 			glPopMatrix(1)

#else /* !__NDS__ */
# if defined(__APPLE__) 
#  include <OpenGL/OpenGL.h>
# else
#  include <GLee.h>
# endif
# define glFrustumf		glFrustum
# define glOrthof		glOrtho
#endif

#include "vec3.h"
#include "mesh.h"

typedef struct r_state {
	uint8_t framebuffer;
	uint8_t renderbuffer;
	uint8_t depth;
	int32_t width;
	int32_t height;
} r_state;

enum element_t {
	R_TRIANGLES = GL_TRIANGLES,
};

void r_bind_buffers(r_state *state);
void r_bind_depthbuffer(r_state *state);
void r_bind_texture(uint16_t id);

void r_clear(float r, float g, float b);
void r_color(float r, float g, float b);
void r_color4(float r, float g, float b, float a);
void r_create_window(uint16_t w, uint16_t h);

void r_free_texture(uint16_t id);

uint32_t r_generate_depthbuffer(int16_t w, int16_t h);
uint32_t r_generate_framebuffer();
uint32_t r_generate_renderbuffer();
float*	 r_generate_texcoords(uint32_t width, uint32_t heigth,
	uint32_t tile_w, uint32_t tile_h);

void r_disable_blending();
void r_disable_culling();
void r_disable_depth();
void r_disable_texcoords();

void r_enable_blending();
void r_enable_culling(int16_t culling);
void r_enable_light(int8_t n);
void r_enable_texcoords(float *coords);

void r_load_identity();
void r_load_matrix(const float m[4][4]);
void r_look_at(float eye_x, float eye_y, float eye_z,
	float center_x, float center_y, float center_z,
	float up_x, float up_y, float up_z);

void r_pop_matrix();
void r_present();
void r_push_matrix();

void r_quit();

void r_render_circle(float radius);
void r_render_cube(float side);
void r_render_line(float x1, float y1, float z1, float x2, float y2, float z2);
void r_render_mesh(mesh *m);
void r_render_quad(float side);
void r_render_ray();
void r_render_sphere(float radius);
void r_render_texquad(float side, float *texcoords, uint32_t offset);
void r_render_triangles(float *vertex, uint16_t vertices, uint16_t *face,
		uint16_t faces);
void r_render_vertices(const float *vertex, uint16_t n);
void r_rotate(float degrees, float x, float y, float z);

void r_scale(float sx, float sy, float sz);
void r_set_clippingarea(int16_t x, int16_t y, int16_t width, int16_t height);
void r_set_light_position(int n, vec3 *position);
void r_set_material(GLenum type, float r, float g, float b);
void r_set_viewport(int32_t x, int32_t y, int32_t w, int32_t h);

void r_setup_ambient_light(int8_t n, float r, float g, float b);
void r_setup_diffuse_light(int8_t n, float r, float g, float b);
void r_setup_orthogonal_view(float width, float height);
void r_setup_perspective_view(float fov, float aspect, float n, float f);

void r_take_screenshot(char *pixels, r_state *buffer);
void r_translate(float x, float y, float z);

uint16_t r_upload_texture(uint16_t w, uint16_t h, int8_t bpp, int8_t type,
    void *image_data);

#endif /* !_RENDERING_H_ */

