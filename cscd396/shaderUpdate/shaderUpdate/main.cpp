#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Cube.h"


#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace glm;
mat4 projection_matrix;
mat4 model_matrix;
mat4 view_matrix;

GLuint projection_loc;
GLuint model_loc;
GLuint view_loc;

GLuint program_gouraud;
GLuint program_phong;
GLuint program_current;

GLuint v, f;
GLboolean show_line = false;

GLfloat aspect = 1.0;

vec4 light_position(10.0, 6.0, 8.0, 1.0);  // directional light source
vec4 light_ambient(0.2, 0.2, 0.2, 1.0);
vec4 light_diffuse(1.0, 1.0, 1.0, 1.0);
vec4 light_specular(1.0, 1.0, 1.0, 1.0);
bool switch_projection = false;
bool switch_shader = false;

// position of the vertices in the hexagon

/*--------------------------------------------------------------------------------------*/
const GLchar* ReadShader(const char* filename);
GLuint LoadShaders(char* v_shader, char* f_shader);
void init(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
//----------------------------------------------------------------------------

const GLchar* ReadShader(const char* filename) {
#ifdef WIN32
	FILE* infile;
	fopen_s(&infile, filename, "rb");

#else
	FILE* infile = fopen(filename, "rb");
#endif // WIN32

	if (!infile) {
#ifdef _DEBUG
		printf("Unable to open file %s", filename);
#endif /* DEBUG */
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = (GLchar*)malloc(sizeof(GLchar)*(len + 1));

	fread(source, 1, len, infile);
	fclose(infile);

	source[len] = 0;

	return ((GLchar*)source);
}

//----------------------------------------------------------------------------

GLuint LoadShaders(char* v_shader, char* f_shader) {

	GLuint p = glCreateProgram();
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	const char * vs = ReadShader(v_shader);
	const char * fs = ReadShader(f_shader);

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

		GLchar* log = (GLchar*)malloc((sizeof(GLchar))*(len + 1));
		glGetShaderInfoLog(v, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free((GLchar*)log);
	}

	glCompileShader(f);

	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc((sizeof(GLchar))*(len + 1));
		glGetShaderInfoLog(f, len, &len, log);
		printf("Fragment Shader compilation failed: %s\n", log);
		free((GLchar*)log);
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

		GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
		glGetProgramInfoLog(p, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free((GLchar*)log);
	}

	return p;

}
////////////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/

void init(void) {
	
	program_gouraud = LoadShaders("shader_gouraud.vert", "shader_gouraud.frag");
	program_phong = LoadShaders("shader_phong.vert", "shader_phong.frag");
	

	glUseProgram(program_gouraud);
	aspect = glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT);

	// Camera position
	view_matrix = glm::lookAt(vec3(0.0f, 0.0f, 6.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	
	// Orthographic projection
	
	projection_matrix = glm::frustum(-4.0f, 4.0f, -4.0f, 4.0f, 4.5f, 100.0f);
	
	// creating an object

	createCube();
	
	// material and light properties
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

}
/****************************************************************/
void Reshape(int width, int height){
	
	glViewport(0, 0, width, height);
	aspect = float(width) / float(height);
}

/****************************************************************************************/
void display(void) {

	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	if (show_line)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (switch_projection)
		projection_matrix = ortho(-4.0f, 4.0f, -4.0f, 4.0f, 4.5f, 100.0f);
	else
		projection_matrix = glm::perspective(radians(70.0f), aspect, 4.5f, 100.0f);


	if (switch_shader)
		program_current = program_gouraud;
	else
		program_current = program_phong;

	glUseProgram(program_current); // uses the current shader
	
	//draws the center cube

	model_matrix = mat4(1.0f);
	model_loc = glGetUniformLocation(program_current, "model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

	projection_loc = glGetUniformLocation(program_current, "projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);
	
	view_loc = glGetUniformLocation(program_current, "view");
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);

	vec4 material_ambient(1.0, 0.0, 1.0, 1.0);
	vec4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	vec4 material_specular(1.0, 0.0, 1.0, 1.0);
	float material_shininess = 5.0;
	vec4 ambient_product = light_ambient*material_ambient;
	vec4 diffuse_product = light_diffuse*material_diffuse;
	vec4 specular_product = light_specular*material_specular;

	glUniform4fv(glGetUniformLocation(program_current, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(program_current, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(program_current, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);

	glUniform4fv(glGetUniformLocation(program_current, "LightPosition"), 1, (GLfloat*)&light_position[0]);
	glUniform1f(glGetUniformLocation(program_current, "Shininess"), material_shininess);

	renderCube();

	// set model matrix to identity matrix
	model_matrix = mat4(1.0f);
	
	// scales and translates the model
	// Draws the top right cube

	mat4 model_matrix_1 = translate(mat4(1.0f), vec3(2.5f, 2.5f, 0.0f));
	mat4 model_matrix_2 = mat4(1.0f); 
	model_matrix = model_matrix_1;
	
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

	material_ambient= vec4(0.0, 1.0, 0.0, 1.0);
	material_diffuse = vec4(0.0, 0.8, 0.0, 1.0);
	material_specular= vec4(1.0, 1.0, 1.0, 1.0);
	material_shininess = 50.0;
	ambient_product = light_ambient*material_ambient;
	diffuse_product = light_diffuse*material_diffuse;
	specular_product = light_specular*material_specular;

	glUniform4fv(glGetUniformLocation(program_current, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(program_current, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(program_current, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);
	glUniform1f(glGetUniformLocation(program_current, "Shininess"), material_shininess);

	renderCube();
	
	//// Draws the top left cube

	// set model matrix to identity matrix
	model_matrix = mat4(1.0f);

	// rotates (around z axis) and translates the model

	model_matrix_1 = translate(mat4(1.0f), vec3(-2.5f, 2.5f, 0.0f));
	model_matrix_2 = rotate(mat4(1.0f), radians(45.0f), vec3(0.0f, 0.0f, 1.0f));
	model_matrix = model_matrix_1*model_matrix_2;

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

	material_ambient = vec4(0.0, 0.0, 1.0, 1.0);
	material_diffuse = vec4(0.0, 0.0, 1.0, 1.0);
	material_specular = vec4(1.0, 1.0, 1.0, 1.0);
	material_shininess = 50.0;
	ambient_product = light_ambient*material_ambient;
	diffuse_product = light_diffuse*material_diffuse;
	specular_product = light_specular*material_specular;

	glUniform4fv(glGetUniformLocation(program_current, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(program_current, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(program_current, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);
	glUniform1f(glGetUniformLocation(program_current, "Shininess"), material_shininess);

	renderCube();

	//// Draws the bottom left cube
	
	// rotates (around x axis) and translates the model

	model_matrix_1 = translate(mat4(1.0f), vec3(-2.5f, -2.5f, 0.0f));
	model_matrix_2 = rotate(mat4(1.0f), radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
	model_matrix = model_matrix_1*model_matrix_2;

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	material_ambient = vec4(0.0, 1.0, 1.0, 1.0);
	material_diffuse = vec4(0.0, 0.8, 1.0, 1.0);
	material_specular = vec4(1.0, 1.0, 1.0, 1.0);
	material_shininess = 50.0;
	ambient_product = light_ambient*material_ambient;
	diffuse_product = light_diffuse*material_diffuse;
	specular_product = light_specular*material_specular;

	glUniform4fv(glGetUniformLocation(program_current, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(program_current, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(program_current, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);
	glUniform1f(glGetUniformLocation(program_current, "Shininess"), material_shininess);

	renderCube();

	//// Draws the bottom right cube
	
	// rotates (around y axis) and translates the model

	model_matrix_1 = translate(mat4(1.0f), vec3(2.5f, -2.5f, 0.0f));
	model_matrix_2 = rotate(mat4(1.0f), radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 model_matrix_3 = scale(mat4(1.0f), vec3(2.0f, 2.0f, 2.0f));
	model_matrix = model_matrix_1*model_matrix_2*model_matrix_3;

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	material_ambient = vec4(1.0, 1.0, 0.0, 1.0);
	material_diffuse = vec4(1.0, 0.8, 0.0, 1.0);
	material_specular = vec4(1.0, 1.0, 1.0, 1.0);
	material_shininess = 50.0;
	ambient_product = light_ambient*material_ambient;
	diffuse_product = light_diffuse*material_diffuse;
	specular_product = light_specular*material_specular;

	glUniform4fv(glGetUniformLocation(program_current, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(program_current, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(program_current, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);
	glUniform1f(glGetUniformLocation(program_current, "Shininess"), material_shininess);

	renderCube();
	
	glutSwapBuffers();
}

/*******************************************************************************************************************/

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'l':
	case 'L':
		show_line = !show_line;
		break;
	case 'p':
	case 'P':
		switch_projection = !switch_projection;
		break;
	case 's':
	case 'S':
		switch_shader = !switch_shader;
		break;
	}
	glutPostRedisplay();
}

/*********************************************************************************************/
/*******************************************************************************************************************/
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Transforming objects");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	printf("%s", glGetString(GL_VERSION));
	
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(Reshape);

	glutMainLoop();
}

/*****/
