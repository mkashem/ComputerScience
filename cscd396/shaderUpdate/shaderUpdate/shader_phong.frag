#version 430 core

// per-fragment interpolated values from the vertex shader

in vec3 N, E, L;
out vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;



void main(void)
{

vec3 Light = normalize(L);
vec3 normal = normalize(N);
vec3 Eye = normalize(E);
vec3 halfVector = normalize(Light+Eye);
vec4 ambient = AmbientProduct;
float Kd = max(dot(Light,normal), 0.0);
vec4 diffuse = Kd*DiffuseProduct;

float Ks = pow(max(dot(normal, halfVector), 0.0), Shininess);
vec4 specular = Ks*SpecularProduct;

color = ambient + diffuse + specular;

color.a = 1.0;


}
