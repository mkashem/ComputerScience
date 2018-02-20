#version 430 core


layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color_loc;
layout (location = 2) in vec4 instance_position_loc;
uniform float pointSize;
uniform float scaleX;
uniform float scaleY;
vec4 pos;
out Fragment
{
    vec4 color;
} fragment;

void main(void){
	gl_PointSize = pointSize;
	pos = position + instance_position_loc;
	pos.x *= scaleX;
	pos.y *= scaleY;
	gl_Position = pos;
    fragment.color = color_loc;
}
