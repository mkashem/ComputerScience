#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
# define GLM_FORCE_RADIANS
#include <glm/mat4x2.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
GLuint matrix_loc;

GLuint  rect_vao;
GLuint  rect_vbo;
GLuint v, f;
GLuint loc;
GLfloat aspect;
static const double kPI = 3.1415926535897932384626433832795;

using namespace std;


GLuint initShaders();
void init();
void display(void);

GLfloat theta;
GLfloat scale;
GLfloat Theta = 45.0;
GLfloat Scale = 0.5;
float angle = 45.0*3.14 / 180;


GLuint initShaders() {

	GLuint p = glCreateProgram();
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vs =
		"#version 430 core\n"

		"in vec4 vPosition;"
		"uniform float theta;"
		"uniform float scale;"
		"float angle = radians(theta);"
		//"mat4 r = mat4( cos(angle), sin(angle), 0.0, 0.0, -sin(angle), cos(angle), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);"
		//"mat4 s = mat4(scale, 0.0, 0.0, 0.0, 0.0, scale, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,0.0, 0.0, 1.0);"
		
		"void main () {"

		//"  gl_Position = r*s*vPosition;"
		"  gl_Position = vPosition;"

		"}";


	const char* fs =
		"#version 430 core\n"

		"out vec4 fColor;"

		"void main () {"

		"  fColor = vec4 (1.0, 0.0, 0.0, 1.0);"

		"}";

	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);

	glCompileShader(v);
	GLint compiled;
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(sizeof(len + 1));
		glGetShaderInfoLog(v, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free(log);
	}

	glCompileShader(f);

	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(sizeof(len + 1));
		glGetShaderInfoLog(f, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free(log);
	}

	glAttachShader(p, v);
	glAttachShader(p, f);
	glBindAttribLocation(p, 0, "vPosition");


	glLinkProgram(p);

	GLint linked;
	glGetProgramiv(p, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLsizei len;
		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(sizeof(len + 1));
		glGetProgramInfoLog(p, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free(log);
	}
	glUseProgram(p);
	return p;

}

void init() {

	GLfloat rect_vertices[4][4] = {
		-0.9f, -0.9f, 0.0, 1.0,
		0.9f, -0.9f, 0.0, 1.0,
		0.9f, 0.9f,  0.0, 1.0,
		-0.9f, 0.9f, 0.0, 1.0
	};

	

	glGenVertexArrays(1, &rect_vao);
	glBindVertexArray(rect_vao);
	glGenBuffers(1, &rect_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rect_vertices), rect_vertices);

	// The following helps you to roatate and scale the vertices
	mat4 model_matrix;

	for (int i = 0; i < 4; i++) {

		GLfloat vert_x = cos(angle)*rect_vertices[i][0] - sin(angle)*rect_vertices[i][1]; 
		GLfloat vert_y = sin(angle)*rect_vertices[i][0] + cos(angle)*rect_vertices[i][1];
		rect_vertices[i][0] = vert_x*0.1; // scale by 0.5
		rect_vertices[i][1] = vert_y*0.1; // scale by 0.5
	//	model_matrix = glm::translate(model_matrix, glm::vec3(0.9f, 0.1f, 0.3f));
		
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rect_vertices), rect_vertices);
	// rotate and scale end 
	GLuint program = initShaders();
	loc = glGetAttribLocation(program, "vPosition");

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(0);

//	theta = glGetUniformLocation(program, "theta");
//	scale = glGetUniformLocation(program, "scale");
	
	glClearColor(1.0, 1.0, 1.0, 1.0);

}

void rotate(int n) {
	angle += 5;
};

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(rect_vao);

	//glUniform1f(theta, Theta);
	//glUniform1f(scale, Scale);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glFlush();
}

/***/
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);

	glutCreateWindow(argv[0]);

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	init();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(display);

	glutMainLoop();
}

/*****/
