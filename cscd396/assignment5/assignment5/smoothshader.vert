#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

struct LightInfo {
  vec4 Position; // Light position in eye coords.
  vec3 La;       // Ambient light intensity
  vec3 Ld;       // Diffuse light intensity
  vec3 Ls;       // Specular light intensity
};
uniform LightInfo Light;

struct MaterialInfo {
  vec3 Ka;            // Ambient reflectivity
  vec3 Kd;            // Diffuse reflectivity
  vec3 Ks;            // Specular reflectivity
  float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;



void main()
{
    vec3 norm;
    vec4 position;

    // Get the position and normal in eye space
     	   norm =normalize(NormalMatrix* VertexNormal);
          position = ModelViewMatrix*vec4(VertexPosition, 1.0);   
    vec3 ambient = Light.La * Material.Ka;

   // Calculate diffuse and specular reflection and add to the light intensity
      
	  
	  
	 vec3 viewDirection= normalize(-position.xyz);
	  vec3 LightDirection = normalize(Light.Position.xyz - position.xyz);
	  vec3 H = normalize(LightDirection + viewDirection);
	  vec3 diffuseLight = Material.Kd*Light.Ld*(max(0.0, dot(LightDirection,norm)));
	 vec3 specular = Material.Ks*Light.Ls*pow(max(dot(norm, H), 0.0), Material.Shininess);
   
    LightIntensity =  ambient+diffuseLight+specular;


    gl_Position = MVP * vec4(VertexPosition,1.0);
}
