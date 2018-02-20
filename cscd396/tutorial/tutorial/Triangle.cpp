
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

GLuint  vao;
GLuint  vbo[2];
GLuint v, f;


GLfloat vertices[3][2] = { { -0.90, -0.90 },  // Triangle 1
{ 0.90, -0.90 },
{ -0.90,  0.90 },
};

GLfloat colors[3][4] = { { 1.0f, 1.0f, 1.0f, 1.0f },
{ 1.0f, 1.0f, 0.0f, 1.0f },
{ 1.0f, 0.0f, 1.0f, 1.0f },
//{ 0.0f, 1.0f, 1.0f, 1.0f },
};


char* ReadFile(const char* filename);
GLuint initShaders(char* v_shader, char* f_shader);
void init();
void display(void);

/************************************************/
char* ReadFile(const char* filename) {
	FILE* infile;

#ifdef WIN32

	fopen_s(&infile, filename, "rb");

#else
	infile = fopen(filename, "rb");
#endif

	if (!infile) {
		printf("Unable to open file %s\n", filename);
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	char* source = (char*)malloc(len + 1);

	fread(source, 1, len, infile);
	fclose(infile);

	source[len] = 0;

	return (source);
}
/*************************************************************/
GLuint initShaders(const char* v_shader, const char* f_shader) {

	GLuint p = glCreateProgram();
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

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

		char* log = (char*)malloc(len + 1);
		glGetShaderInfoLog(v, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free(log);
	}

	glCompileShader(f);

	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &len);

		char* log = (char*)malloc(len + 1);
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

		char* log = (char*)malloc(len + 1);
		glGetProgramInfoLog(p, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free(log);
	}
	glUseProgram(p);
	return p;
}
/*******************************************************/
void init() {

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//glClearColor(1.0, 0.0, 1.0, 0.0);
	glGenBuffers(2, &vbo[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	GLuint program = initShaders("triangle.vs", "triangle.fs");
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	//GLuint program = initShaders("triangle.vs", "triangle.fs");
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

/**********************************************************/
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(vao);

	glDrawArrays(GL_POINTS, 0, 3); //

	glFlush();
}

/*******************************************************/
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Triangle");

	if (glewInit()) {
		printf("%s\n", "Unable to initialize GLEW ...");
	}
	init();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
/*******************************************************/