#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__IPHONE__)
# include <OpenGLES/ES1/gl.h>
# include <OpenGLES/ES1/glext.h>
#elif defined(__ANDROID__) /* !__IPHONE__ */
# include <GLES/gl.h>
# include <GLES/glext.h>
#else /* !__ANDROID__ */
# if defined(__APPLE__) 
#  include <OpenGL/OpenGL.h>
# else
#  include <GLee.h>
# endif
#endif

#include "shader.h"
#include "ifopen.h"

static int32_t compile(int32_t program, const char *filename,
    GLenum type);

int32_t
compile(int32_t program, const char *filename, GLenum type)
{
    int32_t id, status, len;
    char *buffer;
    FILE *fp = ifopen(filename, "rb");

    if (!fp) {
        printf("shader> could not open file: %s\n", filename);
        return 0;
    }

    /* TODO: Move to somewhere common */
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buffer = malloc(len * sizeof(char) + 1);
	fread(buffer, len, 1, fp);
	fclose(fp);

    buffer[len] = 0;

    id = glCreateShader(type);
    /* FIXME: Apparently you should send in lines, but this works o_0 */
    glShaderSource(id, 1, (const GLchar **)&buffer, 0);
    free(buffer);

    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        printf("shader> failed to compile %s\n", filename);
        return 0;
    }
    glAttachShader(program, id);

    return id;
}

int32_t
shader_create(const char *vertex_filename, const char *fragment_filename)
{
    int32_t program, status;

    program = glCreateProgram();
    if (strlen(vertex_filename) > 0
	&& 0 == compile(program, vertex_filename, GL_VERTEX_SHADER)) {
        return 0;
    }

    if (strlen(fragment_filename) > 0
	&& 0 == compile(program, fragment_filename, GL_FRAGMENT_SHADER)) {
        return 0;
    }

    // Link program
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        printf("shader> failed to link %s and %s\n", vertex_filename,
            fragment_filename);
        return 0;
    }

	glUseProgram(program);

    return program;
}

int32_t
shader_get_uniform(int32_t shader, const char *var_name)
{
    int32_t location = glGetUniformLocation(shader, var_name);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("shader> glGetError returned %x\n", err);
		assert(err == GL_NO_ERROR);
	}
    
    if (location < 0) {
        printf("shader> could not get uniform %s\n", var_name);
    }

    return location;
}

void
shader_set_uniform1f(int32_t shader, const char *uniform, float f)
{
    int32_t location = shader_get_uniform(shader, uniform);
    if (location > 0) {
        glUniform1f(location, f);
    }
}

void
shader_set_uniform2f(int32_t shader, const char *uniform, float f1, float f2)
{
    int32_t location = shader_get_uniform(shader, uniform);
    if (location > 0) {
        glUniform2f(location, f1, f2);
    }
}

void
shader_set_uniform3f(int32_t shader, const char *uniform, vec3 *v)
{
    int32_t location = shader_get_uniform(shader, uniform);
    if (location > 0) {
        glUniform3f(location, v->x, v->y, v->z);
    }
}

void
shader_set_uniform4f(int32_t shader, const char *uniform, quat *q)
{
    int32_t location = shader_get_uniform(shader, uniform);
    if (location > 0) {
        glUniform4f(location, q->x, q->y, q->z, q->w);
    }
}

void
shader_use(int32_t shader)
{
    glUseProgram(shader);
}
