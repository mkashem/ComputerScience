
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
GLuint view_matrix_loc;
using namespace glm;
float angle = 45.0*3.14/180;
float tri_look = 45.0*3.14 / 180;
float tri_angle = 45.0*3.14 / 180;
float angle_1 = acos(3.0/sqrt(13.0));
float angle_2 = acos(sqrt(13.0/14.0));
bool change_projection;

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
	printf("%f, %f:", angle_1,angle_2);
	

	matrix_loc = glGetUniformLocation(program, "model_matrix");
	color_loc = glGetUniformLocation(program, "color");

	GLuint offset = 0;
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);
	glGenBuffers(1, &cube_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(cube_vertices), cube_vertices);
	//glBufferSubData(GL_ARRAY_BUFFER, offset+sizeof(cube_vertices), sizeof(positions), cube_vertices);
	
	glGenBuffers(1, &cube_ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


void Display(void){

	// Clear
	glClearColor(0.0, 1.0, 1.0, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Setup
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glBindVertexArray(cube_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);

	// Draw the center cube with shear values of 0.5 along x and y direction

	mat4 model_matrix;
	//mat4 m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 100.0f, 0.0f), vec3(2.0f, 0.0f, 0.0f));
	model_matrix =  mat4(1,0.5,0,0,0.5,1,0,0,0,0,1,0,0,0,0,1);
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	glUniform4fv(color_loc, 1, (GLfloat*)&colors[0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
	mat4 view, projection_matrix;
	//Dfault
	//view = glm::lookAt(vec3(0.0,0.0,0.0), vec3(0.0, 0.0, -100.0), vec3(0.0, 1.0, 0.0)); // this is the default camera positioin 
	view = glm::lookAt(vec3(1.5*cos(radians(tri_look)), 0.0f, 1.5*sin(radians(tri_look))), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0)); // change the camera position 
	projection_matrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f); // enlarge the the view 
	if (change_projection)
		projection_matrix = glm::frustum(-2.0f, 2.0f, -2.0f, 2.0f, 0.65f, 10.0f);
	else
		projection_matrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -3.0f, 3.0f);
	
	view_matrix_loc = glGetUniformLocation(program, "view_matrix");
	glUniformMatrix4fv(glGetUniformLocation(program, "project_matrix"), 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view[0]);
	

	
	mat4  model_matrix_scale, model_matrix_rotate, model_matrix_red, model_matrix_green_t, model_matrix_green, model_matrix_blue, model_matrix_blue_t,model_matrix_yellow;

	//model_matrix_one = glm::scale(mat4(1.0f), vec3(0.5f, -0.6f, 1.0));
	//model_matrix_two = glm::rotate(mat4(1.0f), radians(angle), vec3(0.0f, 0.0f, 1.0f)); // rotate to z 1.0f for x 1.0 0.0 0.0
	 //
	//model_matrix = model_matrix_one*model_matrix_two*model_matrix_three;
	model_matrix = mat4(1.0f); // clear the model_matrix 
	/*kkkkkkkkkkkkkkkkkkkkkkkkkkkk***********************8888888888888***********---------------*/
	// Draw cube translated
	
    model_matrix_red = glm::translate(model_matrix, glm::vec3(-0.6f, -0.6f, 0.0f));
	glUniform4fv(color_loc, 1, (GLfloat*)&colors[4]);
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat *)&model_matrix_red[0]); //
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
	


	// Draw cube scaled
	model_matrix_scale = glm::scale(mat4(1.0f), vec3(1.5f, 1.5f, 1.0));
	model_matrix_green_t = glm::translate(model_matrix, glm::vec3(0.6f, -0.6f, 0.0f));
	model_matrix_green = model_matrix_green_t*model_matrix_scale;
	glUniform4fv(color_loc, 1, (GLfloat*)&colors[8]);
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat *)&model_matrix_green[0]); //
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
	// You'll draw the green cube here
	// position is defined in positions array
	// you need to change the model_matrix
	// you also need to change the color, change the color array index

	
	// Draw cube rotated
	model_matrix_rotate = glm::rotate(mat4(1.0f), radians(angle), vec3(1.0f, 0.0f, 0.0f));
	model_matrix_blue_t = glm::translate(model_matrix, glm::vec3(0.6f, 0.6f, 0.0f));
	model_matrix_blue = model_matrix_blue_t* model_matrix_rotate ;
	glUniform4fv(color_loc, 1, (GLfloat*)&colors[12]);
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat *)&model_matrix_blue[0]); //
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
	/* ********************/
	
mat4 rotate_1, rotate_2, rotate_angle, rotate_2_inverse, rotate_1_inverse, translate_Y_one;
	//translate_one = translate(mat4(1.0f), glm::vec3(0.6f,-0.6f,0.0f));
	rotate_1 = glm::rotate(mat4(1.0f), angle_1, glm::vec3(1.0f,0.0f,0.0f));   // angle_1 = acos(3 / sqrt(13));
	rotate_2 = glm::rotate(mat4(1.0f), -angle_2, glm::vec3(0.0f,1.0f,0.0f));
	rotate_angle = glm::rotate(mat4(1.0f), radians(45.0f), glm::vec3(0.0f,0.0f,1.0f));  // radians(45)
	rotate_2_inverse = glm::rotate(mat4(1.0f), angle_2, glm::vec3(0.0f,1.0f,0.0f));
	rotate_1_inverse = glm::rotate(mat4(1.0f), -angle_1, glm::vec3(1.0f,0.0f,0.0f));
	translate_Y_one = translate(mat4(1.0f), glm::vec3(-0.6f,0.6f,0.0f));
	
	model_matrix = translate_Y_one*rotate_1_inverse*rotate_2_inverse*rotate_angle*rotate_2*rotate_1;
	glUniform4fv(color_loc, 1, (GLfloat*)&colors[16]);
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat *)&model_matrix[0]); //
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
	glEnable(GL_DEPTH_TEST); //
	glDepthFunc(GL_LEQUAL); // 
	

	if (line_mode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	

	glFlush();
}
/**************************************************************************************/

void Rotate(int n) {
	
	switch (n)
	{
	case 1:
		angle += 5;
		glutPostRedisplay();
		glutTimerFunc(100, Rotate, 1);
		break;
	case 2:
		tri_angle += 10;
		glutPostRedisplay();
		glutTimerFunc(100, Rotate, 2); break;
	case 3:
		tri_look += 10;
		glutPostRedisplay();
		glutTimerFunc(100, Rotate, 3);
		break;
	default:
		break;
	} 


};

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'l':case 'L':
		line_mode = !line_mode;
		break;
	case'c':
		change_projection = !change_projection;
	}
	glutPostRedisplay();
}
/**************************************************************************************/


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
	glutTimerFunc(100, Rotate, 1);
	glutTimerFunc(100, Rotate, 2);
	glutTimerFunc(100, Rotate, 3);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}




/*************/



