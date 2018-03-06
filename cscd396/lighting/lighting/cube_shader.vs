#version 430 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat3 NormalMatrix;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

out Fragment
{
    vec4 color;
} fragment;

void main(void)
{


vec3 eyespaceNormal = NormalMatrix*normal;
vec4 eyespacePosition = view_matrix*model_matrix*position;



vec3 N = normalize(eyespaceNormal);

vec3 E = normalize(-eyespacePosition.xyz);

vec3 L = normalize(LightPosition.xyz-eyespacePosition.xyz);

vec3 H = normalize(L+E);


vec4 ambient = AmbientProduct;
vec4 diffuse = DiffuseProduct* (max(dot(L,N), 0.0));
vec4 specular = SpecularProduct*( pow(max(dot(N, H), 0.0), Shininess));

gl_Position = projection_matrix*view_matrix*model_matrix*position;

fragment.color = ambient + diffuse + specular;
    

}

