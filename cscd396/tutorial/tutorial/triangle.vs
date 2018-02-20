#version 430 core

in vec4 vPosition;
layout(location =0 ) in vec4 position;
layout(location =1 ) in vec4 color;
out vec4 vs_fs_colors_out;

void main(){


     gl_Position = vPosition;
     gl_PointSize = 50.0;
	 vs_fs_colors_out = color;

}
