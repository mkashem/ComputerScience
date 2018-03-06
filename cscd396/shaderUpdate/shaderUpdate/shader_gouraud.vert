#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;
out vec4 fColor;


void main(){

 // Normalize the input lighting vectors

   vec3 N = normalize(normal);
   vec3 E = normalize(position.xyz);
   vec3 L = normalize(LightPosition.xyz);

   vec3 H = normalize(L+E);

vec4 ambient = AmbientProduct;
float Kd = max(dot(L,N), 0.0);
vec4 diffuse = Kd*DiffuseProduct;

float Ks = pow(max(dot(N, H), 0.0), Shininess);
vec4 specular = Ks*SpecularProduct;

fColor = ambient + diffuse + specular;

fColor.a = 1.0;

     gl_Position = projection*view*model*position;
}
