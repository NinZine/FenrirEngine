varying vec3 normal;
varying vec4 position;

void main(void)
{
   normal = normalize(gl_NormalMatrix * gl_Normal);
   position = gl_ModelViewMatrix * gl_Vertex;
   gl_TexCoord[0].st = gl_MultiTexCoord0.st;
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}