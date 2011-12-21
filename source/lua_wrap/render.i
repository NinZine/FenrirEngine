%module render

%{
	#include "rendering.h"
%}

%rename(state) r_state;

%rename(bind_buffers) r_bind_buffers;
%rename(bind_depthbuffer) r_bind_depthbuffer;
%rename(bind_texture) r_bind_texture;
%rename(clear) r_clear;
%rename(color) r_color;
%rename(color4) r_color4;
%rename(create_window) r_create_window;
%rename(generate_depthbuffer) r_generate_depthbuffer;
%rename(generate_framebuffer) r_generate_framebuffer;
%rename(generate_renderbuffer) r_generate_renderbuffer;
%rename(generate_texcoords) r_generate_texcoords;
%rename(disable_blending) r_disable_blending;
%rename(disable_culling) r_disable_culling;
%rename(disable_depth) r_disable_depth;
%rename(disable_texcoords) r_disable_texcoords;
%rename(enable_blending) r_enable_blending;
%rename(enable_culling) r_enable_culling;
%rename(enable_depth) r_enable_depth;
%rename(enable_light) r_enable_light;
%rename(enable_texcoords) r_enable_texcoords;
%rename(free_texture) r_free_texture;
%rename(load_identity) r_load_identity;
%rename(load_matrix) r_load_matrix;
%rename(look_at) r_look_at;
%rename(matrix_mode) r_matrix_mode;
%rename(pop_matrix) r_pop_matrix;
%rename(present) r_present;
%rename(push_matrix) r_push_matrix;
%rename(quit) r_quit;
%rename(render_circle) r_render_circle;
%rename(render_cube) r_render_cube;
%rename(render_line) r_render_line;
%rename(render_mesh) r_render_mesh;
%rename(render_quad) r_render_quad;
%rename(render_ray) r_render_ray;
%rename(render_sphere) r_render_sphere;
%rename(render_texquad) r_render_texquad;
%rename(render_triangles) r_render_triangles;
%rename(render_vertices) r_render_vertices;
%rename(rotate) r_rotate;
%rename(scale) r_scale;
%rename(set_clippingarea) r_set_clippingarea;
%rename(set_light_position) r_set_light_position;
%rename(set_material) r_set_material;
%rename(set_viewport) r_set_viewport;
%rename(setup_ambient_light) r_setup_ambient_light;
%rename(setup_diffuse_light) r_setup_diffuse_light;
%rename(setup_orthogonal_view) r_setup_orthogonal_view;
%rename(setup_perspective_view) r_setup_perspective_view;
%rename(take_screenshot) r_take_screenshot;
%rename(translate) r_translate;
%rename(upload_texture) r_upload_texture;

%include <stdint.i>
//%include <carrays.i>
//%array_functions(float, uint16_t);

%include "../../include/rendering.h"

