#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


GLuint  cube_vao;
GLuint  cube_vbo;
GLuint  cube_ebo;

GLuint color_loc;
GLuint matrix_loc;

GLuint program;
GLboolean line_mode = false;
static const double kPI = 3.1415926535897932384626433832795;
using namespace glm;

static const GLfloat cube_vertices[] = { 0.15f, 0.15f, 0.15f, 1.0f,   // v0
                                        -0.15f, 0.15f, 0.15f, 1.0f,   // v1
                                        -0.15f, -0.15f, 0.15f, 1.0f,  // v2
                                         0.15f, -0.15f, 0.15f, 1.0f,  // v3
                                         0.15f, -0.15f, -0.15f, 1.0f, // v4
                                         0.15f, 0.15f, -0.15f, 1.0f,  // v5
                                        -0.15f, 0.15f, -0.15f, 1.0f,  // v6
                                         -0.15f, -0.15f, -0.15f, 1.0f,// v7
                                       };

static const GLushort cube_indices[] = { 0, 1, 2, 0, 2, 3,  // front
                                         0, 3, 4, 0, 4, 5,  // right
                                         0, 5, 6, 0, 6, 1,  // top
                                         1, 2, 6, 2, 7, 6,  // left
                                         4, 7, 2, 4, 2, 3,  // bottom
                                         5, 6, 7, 4, 5, 7 };// back

static const GLfloat colors[] = { 1.0f, 1.0f, 1.0f, 1.0f,
                                  1.0f, 0.0f, 0.0f, 1.0f,
                                  0.0f, 1.0f, 0.0f, 1.0f,
                                  0.0f, 0.0f, 1.0f, 1.0f,
                                  1.0f, 1.0f, 0.0f, 1.0f,};

static const GLfloat positions[] = { 0.0f, 0.0f, 0.0f,
                                    -0.6f, -0.6f, 0.0f,
                                     0.6f, -0.6f, 0.0f,
                                     0.6f, 0.6f, 0.0f, 
                                    -0.6f, 0.6f, 0.0f,};

static const GLchar* ReadFile(const char* filename);
GLuint initShaders(const char* v_shader, const char* f_shader);
void Initialize();
void Display(void);

static const GLchar* ReadFile(const char* filename)
{
	FILE* infile;

#ifdef WIN32

	fopen_s(&infile, filename, "rb");

#else
	infile = fopen(filename, "r");
#endif
	if (!infile) {
		printf("Unable to open file %s\n", filename);
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = (GLchar*)malloc(len + 1);
	fread(source, 1, len, infile);
	fclose(infile);
	source[len] = 0;

	return (GLchar*) (source);
}

GLuint initShaders(const char *v_shader, const char *f_shader) {

	GLuint p = glCreateProgram();
	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

	const char * vs = ReadFile(v_shader);
	const char * fs = ReadFile(f_shader);

	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);
	free((char*)vs);
	free((char*)fs);

	glCompileShader(v);
	GLint compiled;
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(len + 1);
		glGetShaderInfoLog(v, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n",log);
		free(log);
	}

	glCompileShader(f);

	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(len + 1);
		glGetShaderInfoLog(f, len, &len, log);
		printf("Fragment Shader compilation failed: %s\n", log);
		free(log);
	}

	glAttachShader(p, v);
	glAttachShader(p, f);
	glLinkProgram(p);

	GLint linked;
	glGetProgramiv(p, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLsizei len;
		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(len + 1);
		glGetProgramInfoLog(p, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free(log);
	}
	glUseProgram(p);
	return p;

}

void Initialize(void) {

	// Create the program for rendering the model

	program = initShaders("cube_shader.vs", "cube_shader.fs");

	// uniform indices

	matrix_loc = glGetUniformLocation(program, "model_matrix");
	color_loc = glGetUniformLocation(program, "color");

	GLuint offset = 0;
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);
	glGenBuffers(1, &cube_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(cube_vertices), cube_vertices);
	
	glGenBuffers(1, &cube_ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


void Display(void){

	// Clear

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Setup
	
	glBindVertexArray(cube_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);

	// Draw the center cube with shear values of 0.5 along x and y direction

	mat4 model_matrix;
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	glUniform4fv(color_loc, 1, (GLfloat*)&colors[0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);


	// Draw cube translated

	// You'll draw the red cube here
	// position is defined in positions array
	// you need to change the model_ matrix
	// you also need to change the color, change the color array index

	
	// Draw cube scaled

	// You'll draw the green cube here
	// position is defined in positions array
	// you need to change the model_matrix
	// you also need to change the color, change the color array index

	
	// Draw cube rotated

	// You'll draw the blue cube here
	// position is defined in positions array
	// you need to change the model_matrix
	// you also need to change the color, change the color array index
	

	// Draw the cube rotated about an angle of 45 degree about an arbitrary axis (1.0f, 2.0f, 3.0f)

	// You'll draw the yellow cube here
	// position is defined in positions array
	// you need to change the model_ matrix
	// you also need to change the color, change the color array index

	

	glFlush();
}
/**************************************************************************************/
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'l':case 'L':
		line_mode = !line_mode;
		break;
	}
	glutPostRedisplay();
}
/**************************************************************************************/

/*********/
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Transformed Cube");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}




/*************/



