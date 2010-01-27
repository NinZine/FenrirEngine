%module mat4
%{
    #define SWIG_init_user SWIG_init_user_mat4
    #include "mat4.h"
%}

%rename(get) mat4_get;
%rename(set) mat4_set;
%rename(copy) mat4_copy;
%rename(determinant) mat4_determinant;
%rename(identity) mat4_identity;
%rename(inverse) mat4_inverse;
%rename(lerp) mat4_lerp;
%rename(mul) mat4_mul;
%rename(mul_vec3) mat4_mul_vec3;
%rename(print) mat4_print;
%rename(reset) mat4_reset;
%rename(rotate) mat4_rotate;
%rename(rotation) mat4_rotation;
%rename(transpose) mat4_transpose;

%inline %{
float
mat4_get(mat4 *m, int x, int y)
{
    return m->m[y][x];
}

void
mat4_set(mat4 *m, int x, int y, float f)
{
    m->m[y][x] = f;
}
%}

%include "../../include/mat4.h"

