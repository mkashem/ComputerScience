#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

out vec3 N;
out vec3 L;
out vec3 E;


void main(){

 // Normalize the input lighting vectors

   N = normal;
   E = position.xyz;
   L = LightPosition.xyz;
   gl_Position = projection*view*model*position;

}
