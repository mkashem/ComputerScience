#version 430



in vec3 norm;
in vec4 position;
out vec4 FragColor;
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

void main()
{
    

	 vec3 viewDirection= normalize(-position.xyz);
	  vec3 ambient = Light.La * Material.Ka;
	  vec3 LightDirection = normalize(Light.Position.xyz - position.xyz);
	  vec3 H = normalize(LightDirection + viewDirection);
	  vec3 diffuseLight = Material.Kd*Light.Ld*(max(0.0, dot(LightDirection,norm)));
	 vec3 specular = Material.Ks*Light.Ls*pow(max(dot(norm, H), 0.0), Material.Shininess);
   
    LightIntensity =  ambient+diffuseLight+specular;
   FragColor = vec4(LightIntensity,1.0);
}
