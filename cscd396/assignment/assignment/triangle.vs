#version 430 core

in vec4 vPosition;
void main(){


     gl_Position = vPosition;
     gl_PointSize = 50.0;

}
