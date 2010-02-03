%module render

%{
	#define SWIG_init_user SWIG_init_user_render
	#include "rendering.h"
%}

%rename(state) r_state;

%rename(bind_buffers) r_bind_buffers;
%rename(bind_depthbuffer) r_bind_depthbuffer;
%rename(clear) r_clear;
%rename(color) r_color;
%rename(generate_depthbuffer) r_generate_depthbuffer;
%rename(generate_framebuffer) r_generate_framebuffer;
%rename(generate_renderbuffer) r_generate_renderbuffer;
%rename(disable_culling) r_disable_culling;
%rename(enable_culling) r_enable_culling;
%rename(enable_light) r_enable_light;
%rename(load_identity) r_load_identity;
%rename(pop_matrix) r_pop_matrix;
%rename(present) r_present;
%rename(push_matrix) r_push_matrix;
%rename(render_circle) r_render_circle;
%rename(render_cube) r_render_cube;
%rename(render_quad) r_render_quad;
%rename(render_ray) r_render_ray;
%rename(render_sphere) r_render_sphere;
%rename(render_vertices) r_render_vertices;
%rename(rotate) r_rotate;
%rename(set_clippingarea) r_set_clippingarea;
%rename(set_light_position) r_set_light_position;
%rename(set_material) r_set_material;
%rename(setup_ambient_light) r_setup_ambient_light;
%rename(setup_diffuse_light) r_setup_diffuse_light;
%rename(setup_orthogonal_view) r_setup_orthogonal_view;
%rename(setup_perspective_view) r_setup_perspective_view;
%rename(take_screenshot) r_take_screenshot;
%rename(translate) r_translate;

%include <stdint.i>
%include "../../include/rendering.h"

