#version 430 core

// per-fragment interpolated values from the vertex shader


in vec4 fColor;
out vec4 color;

void main(void)
{




color = fColor;
}
