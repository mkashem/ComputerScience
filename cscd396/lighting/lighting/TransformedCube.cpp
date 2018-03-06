#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace glm;
GLuint  cube_vao;
GLuint  cube_vbo;
GLuint  cube_ebo;

GLuint matrix_loc;
GLuint view_matrix_loc;

GLfloat angle_rotate = 0.0;
GLfloat angle_lookup = 0.0;
GLfloat tri_angle = 0.0;
GLuint projection_matrix_loc;
GLboolean wire_frame = false;
glm::mat4 projection_matrix;
GLuint program;
GLboolean change_projection = false;

static const double kPI = 3.1415926535897932384626433832795;
using namespace glm;

glm::mat4 view;

GLfloat cube_vertices[] = { -0.5, -0.5, 0.5, 1.0f, //Front
0.5, -0.5, 0.5, 1.0f,
0.5, 0.5, 0.5, 1.0f,
-0.5, 0.5, 0.5, 1.0f,

0.5, -0.5, 0.5, 1.0f, //Right
0.5, -0.5, -0.5, 1.0f,
0.5, 0.5, -0.5, 1.0f,
0.5, 0.5, 0.5, 1.0f,

-0.5, -0.5, -0.5, 1.0f, //Back
-0.5, 0.5, -0.5, 1.0f,
0.5, 0.5, -0.5, 1.0f,
0.5, -0.5, -0.5, 1.0f,
//Left
-0.5, -0.5, 0.5, 1.0f,
-0.5, 0.5, 0.5, 1.0f,
-0.5, 0.5, -0.5, 1.0f,
-0.5, -0.5, -0.5, 1.0f,
//Bottom
-0.5, -0.5, 0.5, 1.0f,
-0.5, -0.5, -0.5, 1.0f,
0.5, -0.5, -0.5, 1.0f,
0.5, -0.5,  0.5, 1.0f,
//Top
-0.5, 0.5, 0.5, 1.0f,
0.5, 0.5, 0.5, 1.0f,
0.5, 0.5, -0.5, 1.0f,
-0.5, 0.5, -0.5, 1.0f
};

float normals[] = {
	// Front
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	// Right
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	// Back
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	// Left
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	// Bottom
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	// Top
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};

GLushort cube_indices[] = {
	0, 1, 2, 0, 2, 3,
	4, 5, 6, 4, 6, 7,
	8, 9, 10, 8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23
};

float material_shininess = 50.0;
vec4 light_position(10.0, 10.0, 9.8, 1.0); // positional light source
vec4 light_ambient(0.3, 0.3, 0.3, 1.0);
vec4 light_diffuse(1.0, 1.0, 1.0, 1.0);
vec4 light_specular(1.0, 1.0, 1.0, 1.0);

vec4 material_ambient(1.0, 1.0, 0.0, 1.0);
vec4 material_diffuse(1.0, 0.8, 0.0, 1.0);
vec4 material_specular(1.0, 0.0, 1.0, 1.0);

static const GLchar* ReadFile(const char* filename);
GLuint initShaders(const char* v_shader, const char* f_shader);
void Initialize();
void Display(void);

static const GLchar* ReadFile(const char* filename){
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
	
	// uniform indices
	projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");
	
	GLuint offset = 0;
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);
	glGenBuffers(1, &cube_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_vertices), cube_vertices);
	// add normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices) + sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(cube_vertices), cube_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_vertices), sizeof(normals), normals);

	glGenBuffers(1, &cube_ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Add normal arttrib pointer
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(cube_vertices)));
	glEnableVertexAttribArray(1);
	//Default 
	view = glm::lookAt(vec3( 0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	
	// Compute light position in eye coordinate
	
	
	view_matrix_loc = glGetUniformLocation(program, "view_matrix");
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view[0]);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Display(void){

	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glLineWidth(20.0);

	if (wire_frame)
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(cube_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
// consider material ambient
	vec4 material_ambient(1.0, 0.0, 1.0, 1.0);
	vec4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	vec4 material_specular(1.0, 0.0, 1.0, 1.0);


	// compute total ambient, diffuse and specular
	vec4 ambient_product = light_ambient*material_ambient;
	vec4 diffuse_product = light_diffuse*material_diffuse;
	vec4 specular_product = light_specular*material_specular;


	//vec4 ambient_product = light_ambient*material_ambient;
	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	//glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	if (change_projection)
		//projection_matrix = glm::perspective(radians(90.0f), 1.0f, 0.65f, 10.0f);
		projection_matrix = glm::frustum(-2.0f, 2.0f, -2.0f, 2.0f, 1.5f, 10.0f);
	else
		projection_matrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -8.0f, 8.0f);
	view = glm::lookAt(vec3(3.0f*cos(radians(angle_lookup)), 0.0f, 3.0f*sin(radians(angle_lookup))), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view[0]);

	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);
	
	// Add uniform variables for ambient, diffuse, specular, shininess and light position
	
	// Draw the center cube
	GLfloat angle = 45.0f*kPI / 180;
	mat4 model_matrix;
	
	model_matrix = mat4(1, 0.5, 0, 0, 0.5, 1.0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	
	// Compute normal matrix and add as a uniform variable
	mat3 normal = mat3(view*model_matrix);
	glUniformMatrix3fv(glGetUniformLocation(program, "NormalMatrix"), 1, GL_FALSE,
		(GLfloat*)&normal[0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
	
	
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);

	
	glFlush();
}

/**************************************************************************************/
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'u':case 'U':
		wire_frame = !wire_frame;
		break;
	case 'c':case 'C':
		change_projection = !change_projection;
		break;
	}

	glutPostRedisplay();
}
/*********************************************************************************************/
/*************************************************************************/
void Rotate(int n)  //the "glutTimerFunc"
{
	switch (n) {
	case 1:
		angle_rotate += 5;
		glutPostRedisplay();
		glutTimerFunc(100, Rotate, 1);
		break;
	case 2:
		tri_angle -= 10;
		glutPostRedisplay();
		glutTimerFunc(100, Rotate, 2);
		break;
	case 3:
		angle_lookup -= 5;
		glutPostRedisplay();
		glutTimerFunc(100, Rotate, 3);
		break;
	}
}
/*********/
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Well Lit Cube");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutTimerFunc(100, Rotate, 1);
	glutTimerFunc(100, Rotate, 2);
	glutTimerFunc(100, Rotate, 3);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}

/*************/



