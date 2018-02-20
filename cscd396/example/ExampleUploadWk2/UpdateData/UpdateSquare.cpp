#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

void Initialize(void);
void Display(void);

GLuint render_prog;
GLuint              square_vao;
GLuint              square_vbo;
bool show_line = false;
bool update_vertices = false;

typedef struct {
	GLenum       type;
	const char*  filename;
	GLuint       shader;
} ShaderInfo;


GLfloat square_vertices[] =
{
	-0.70f, -0.70f, 0.0f, 1.0f,
	0.70f, -0.70f, 0.0f, 1.0f,
	0.70f, 0.70f, 0.0f, 1.0f,
	-0.70f, 0.70f, 0.0f, 1.0f
};

GLfloat square_vertices_updated[] =
{
	-0.90f, -0.90f, 0.0f, 1.0f,
	0.90f, -0.90f, 0.0f, 1.0f,
	0.90f, 0.90f, 0.0f, 1.0f,
	-0.90f, 0.90f, 0.0f, 1.0f
};

GLfloat colors[] =
{
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
};

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

GLuint
LoadShaders(ShaderInfo* shaders)
{
	if (shaders == NULL) { return 0; }

	GLuint program = glCreateProgram();

	ShaderInfo* entry = shaders;
	while (entry->type != GL_NONE) {
		GLuint shader = glCreateShader(entry->type);

		entry->shader = shader;

		const GLchar* source = ReadShader(entry->filename);

		if (source == NULL) {


			for (entry = shaders; entry->type != GL_NONE; ++entry) {
				glDeleteShader(entry->shader);
				entry->shader = 0;
			}

			return 0;
		}

		glShaderSource(shader, 1, &source, NULL);
		free((GLchar*)source);

		glCompileShader(shader);

		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
#ifdef _DEBUG
			GLsizei len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
			glGetShaderInfoLog(shader, len, &len, log);
			printf("Shader compilation failed.\n");
			free(log);
#endif /* DEBUG */

			return 0;
		}

		glAttachShader(program, shader);

		++entry;
	}

	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
#ifdef _DEBUG
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
		glGetProgramInfoLog(program, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free(log);
#endif /* DEBUG */

		for (entry = shaders; entry->type != GL_NONE; ++entry) {
			glDeleteShader(entry->shader);
			entry->shader = 0;
		}

		return 0;
	}

	return program;
}
/******************************************************************************/
void Initialize(void)
{
	// Create the program for rendering the model
	render_prog = glCreateProgram();

	ShaderInfo shaders[] = { { GL_VERTEX_SHADER, "square.vs" },
	                         { GL_FRAGMENT_SHADER, "square.fs" },
	                         { GL_NONE, NULL }
	                       };

	render_prog = LoadShaders(shaders);
	glUseProgram(render_prog);

	GLuint offset = 0;
	glGenVertexArrays(1, &square_vao);
	glGenBuffers(1, &square_vbo);
	glBindVertexArray(square_vao);
	glBindBuffer(GL_ARRAY_BUFFER, square_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(square_vertices), square_vertices);
	offset += sizeof(square_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(colors), colors);
	offset += sizeof(colors);
	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(square_vertices));
	glEnableVertexAttribArray(1);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}
/*********************************************************************/
void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 's':
		show_line = !show_line;
		break;

	case 'u':
		update_vertices = !update_vertices;
		break;
	}
	glutPostRedisplay();
}
/***********************************************************************/
void Display(void){
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBindVertexArray(square_vao);
	if (update_vertices)
	    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(square_vertices_updated), square_vertices_updated);
	else
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(square_vertices), square_vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glFlush();

}

/*****************************************/
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutCreateWindow("Updated Square");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
}

/********************************************/