#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 norm;
out vec4 position;


uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;



void main()
{
    

    // Get the position and normal in eye space
     	   norm =normalize(NormalMatrix* VertexNormal);
          position = ModelViewMatrix*vec4(VertexPosition, 1.0);   
    


    gl_Position = MVP * vec4(VertexPosition,1.0);
}
