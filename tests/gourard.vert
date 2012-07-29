void main() {
   vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
   gl_Position = gl_ModelViewMatrix * gl_Vertex;
   gl_Position = gl_Position / gl_Position.w;
   float NdotL; //cosin normal lightDir
   float HdotN; //cosin half way vector normal
   vec3 lightDir;
   vec3 halfVector;
   vec4 diffuseC;
   vec4 specularC;
   int numLightSources = 1;
   gl_FrontColor = gl_FrontMaterial.ambient;
   for(int i = 0; i < numLightSources; i++) {
      lightDir = normalize(vec3(gl_LightSource[i].position));
      NdotL = max(dot(normal, lightDir), 0.0);
      diffuseC = gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse * NdotL;
      gl_FrontColor += diffuseC;
      if (NdotL > 0.0) {
         halfVector = normalize(lightDir - normalize(gl_Position.xyz));
         HdotN = max(0.0, dot(halfVector,  normal));
         specularC = gl_FrontMaterial.specular * gl_LightSource[i].specular * pow (HdotN, gl_FrontMaterial.shininess);
         gl_FrontColor += specularC;
      }
   }
   gl_Position = gl_ProjectionMatrix * gl_Position;
}

