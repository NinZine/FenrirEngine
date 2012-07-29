uniform vec3 lightPosition;
//uniform vec3 directionalLight;

uniform vec4 lightColor;
uniform vec4 ambient;
uniform float shininess;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;

//uniform sampler2D Texture0;

varying vec3 normal;
varying vec4 position;

void main(void)
{
	vec3 lightVector = lightPosition - position.xyz;
	//vec3 lightVector = directionalLight;
    vec3 eyeVector = normalize((gl_ModelViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0)) + position).xyz;

	vec3 norm = normalize(normal);
	lightVector = normalize(lightVector);

	vec3 diffuse = max(dot(norm, lightVector), 0.0) * (materialDiffuse * lightColor.xyz);
	vec3 half_vector = normalize(lightVector - eyeVector);
	vec3 specular = pow(dot(norm, half_vector), shininess) * (materialSpecular * lightColor.xyz);

	//vec3 texColor = texture2D(Texture0, gl_TexCoord[0].st).xyz;
	vec3 texColor = vec3(0.0, 0.0, 0.0);

	gl_FragColor = vec4(ambient.xyz + specular + diffuse + texColor, 1.0 );
}
