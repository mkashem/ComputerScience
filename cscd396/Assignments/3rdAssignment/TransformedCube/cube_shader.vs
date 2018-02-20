#version 430 core

layout (location = 0) in vec4 position;

uniform vec4 color;
uniform mat4 model_matrix;


out Fragment
{
    vec4 color;
} fragment;

void main(void)
{
    gl_Position = model_matrix*position;
    fragment.color = color;

}

