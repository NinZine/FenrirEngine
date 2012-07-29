#include <stdint.h>

#include "vec3.h"
#include "quat.h"

int32_t     shader_create(const char *vertex_file, const char *fragment_file);
int32_t     shader_get_uniform(int32_t shader, const char *var_name);
void        shader_set_uniform1f(int32_t shader, const char *uniform, float f);
void        shader_set_uniform2f(int32_t shader, const char *uniform,
				float f1, float f2);
void        shader_set_uniform3f(int32_t shader, const char *uniform, vec3 *v);
void        shader_set_uniform4f(int32_t shader, const char *uniform, quat *q);
void        shader_use(int32_t shader);
