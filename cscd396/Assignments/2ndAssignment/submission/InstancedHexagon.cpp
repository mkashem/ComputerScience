#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

GLuint render_prog;
GLuint  hex_vao;
GLuint  hex_vbo;
GLuint  ebo;
GLboolean update_hex;
GLboolean show_line;
GLboolean point;
GLboolean scale_mode;
GLfloat pointSize; // point size 
GLint timeLoc; // for index
GLfloat  pSizeMax;
GLfloat  pSizeMin;
GLint scaleX_loc; // for index
GLint scaleY_loc; // for index
GLfloat scaleVartex_X =1.0;
GLfloat scaleVartex_Y =1.0;
GLfloat Lsize[2];
GLfloat LineSize;

typedef struct {
	GLenum       type;
	const char*  filename;
	GLuint       shader;
} ShaderInfo;


// position of the vertices in the hexagon

GLfloat hex_vertices[] = {

	0.0, 0.0, 0.0, 1.0,             // index '0'
	-0.1125, -0.1875, 0.0, 1.0,     // index '1'
	0.1125, -0.1875, 0.0, 1.0,      // index '2'

	//0.0, 0.0, 0.0, 1.0,             // index '0'
	//0.1125, -0.1875, 0.0, 1.0,      // index '2'
	0.225, 0.0, 0.0, 1.0,           // index '3'

	//0.0, 0.0, 0.0, 1.0,             // index '0'
	//0.225, 0.0, 0.0, 1.0,           // index '3'
	0.1125, 0.1875, 0.0, 1.0,       // index '4'

	//0.0, 0.0, 0.0, 1.0,             // index '0'
	//0.1125, 0.1875, 0.0, 1.0,       // index '4'
	-0.1125, 0.1875, 0.0, 1.0,      // index '5'

	//0.0, 0.0, 0.0, 1.0,             // index '0'
//	-0.1125, 0.1875, 0.0, 1.0,      // index '5'
	-0.225, 0.0, 0.0, 1.0,          // index '6'

//	0.0, 0.0, 0.0, 1.0,             // index '0'
//	-0.1125, -0.1875, 0.0, 1.0,     // index '6'
//	-0.225, 0.0, 0.0, 1.0           // index '1'
};


GLfloat hex_vertices_update[] = {
	0.0, 0.0, 0.0, 1.0,              // index '0'
	-0.1125, -0.1875, 0.0, 1.0,      // index '1'
	0.1125, -0.1875, 0.0, 1.0,       // index '2'
	0.1125, 0.0, 0.0, 1.0,           // index '3'
	0.1125, 0.1875, 0.0, 1.0,        // index '4'
	-0.1125, 0.1875, 0.0, 1.0,       // index '5'
	-0.1125, 0.0, 0.0, 1.0           // index '6'
};

// color of the vertices

GLfloat hex_colors[] = {
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f
};

// position of the hexagon
GLfloat hex_positions[] =
{
	0.0f, 0.0f, 0.0f, 1.0f,
	-0.40f, -0.60f, 0.0f, 1.0f,
	0.40f, -0.60f, 0.0f, 1.0f,
	0.40f, 0.60f, 0.0f, 1.0f,
	-0.40f, 0.60f, 0.0f, 1.0f,
	-0.90f, 0.0f, 0.0f, 1.0f,
	0.90f, 0.0f, 0.0f, 1.0f
};
/*glElement start*/
GLushort indices[] =
{
	0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 1
};
/*glElement start*/

void Initialize(void);
void Display(void);

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
//-----------------------------------------------------------------------------

GLuint LoadShaders(ShaderInfo* shaders)
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

/***********************************************************************************/
void Initialize(void)
{
	// Create the program for rendering the model

	ShaderInfo shaders[] = { { GL_VERTEX_SHADER, "instancing.vs" },
	{ GL_FRAGMENT_SHADER, "instancing.fs" },
	{ GL_NONE, NULL }
	};

	render_prog = LoadShaders(shaders);
	glUseProgram(render_prog);
	// points 
	glEnable(GL_PROGRAM_POINT_SIZE);
	glGetFloatv(GL_POINT_SIZE_MAX, &pSizeMax); // not working 
	if (pSizeMax > 255.0) {
		pSizeMax = 255.0;
	}
	
	
	//glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, &pSizeMax);
	glGetFloatv(GL_POINT_SIZE_MIN, &pSizeMin);
	//printf("MIN %f", pSizeMin);
	glGetFloatv(GL_LINE_WIDTH_RANGE, Lsize);
	printf(" min %f", Lsize[0]);
	printf(" max %f", Lsize[1]);
	scaleX_loc = glGetUniformLocation(render_prog, "scaleX"); // for scaling 
	scaleY_loc = glGetUniformLocation(render_prog, "scaleY"); // for scaling scaleY variable from vertex shader file.

	// point end 
	
	glGenVertexArrays(1, &hex_vao);
	glGenBuffers(1, &hex_vbo);
	glBindVertexArray(hex_vao);
	glBindBuffer(GL_ARRAY_BUFFER, hex_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hex_vertices)+sizeof(hex_colors)+sizeof(hex_positions), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hex_vertices), hex_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hex_vertices), sizeof(hex_colors), hex_colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hex_vertices)+sizeof(hex_colors),sizeof(hex_positions),hex_positions);
	/*for instancing start*/
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(hex_vertices)); // forth position 0 is stride .
	glEnableVertexAttribArray(1); 
	glVertexAttribDivisor(1, 1); // every instance of color will change
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(hex_vertices) + sizeof(hex_colors))); // pointing to the (GLvoid*)(sizeof(hex_vertices) + sizeof(hex_colors) pointer 
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1); //every instance of position will change
	/*instancing end*/
	/* glElement start, set up element array buffer*/
	glGenBuffers(1, &ebo); // ebo is id; 1 is number of objects
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	/* glElement end*/
	//start


	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	/**/
	/* Uniform index for variable "time" in shader*/
	//GLfloat timeValue; /* Application time */
	timeLoc = glGetUniformLocation(render_prog, "pointSize"); // return index of pointSIze from shader 
	glEnable(GL_PROGRAM_POINT_SIZE);
	//glUniform1f(timeLoc, timeValue);
	//end
}

/**************************************************************************************/
void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'u':case 'U':
		update_hex = !update_hex;
		break;
	case 'l':case 'L':
		show_line= !show_line;
		LineSize = (rand() % int(Lsize[1] + 1));
		break;
	case 'p':case 'P':
		pointSize = (rand() % int(pSizeMax + 1)); //
		point = !point;
		
		break;
	case 's':case 'S':
		scale_mode = !scale_mode;
		scaleVartex_X = (rand() %11 +1)*0.1;
		scaleVartex_Y = (rand() % 11 + 1)*0.1;
		break;

	
	}
	glutPostRedisplay();
}
/**************************************************************************************/

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	

	glBindVertexArray(hex_vao);
	/*for update start*/
	if (!update_hex) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hex_vertices), hex_vertices);
	}	
	else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hex_vertices_update), hex_vertices_update);
	}		
	/*update end */
	/************For wireframe coding start bellow************/
	if (!show_line)

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else {
		glLineWidth(LineSize);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
		
	// glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
	
	/****************wireframe end***********/

	/*point size update*/

	if (point) {
		glUniform1f(timeLoc, pointSize);
		glDrawElementsInstanced(GL_POINTS, 18, GL_UNSIGNED_SHORT, 0, 7);
	}
	else
		glDrawElementsInstanced(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0, 7);

	/*point size update end */

	
		
	//glUniform1f(timeLoc, 50.0);
	glUniform1f(scaleX_loc, scaleVartex_X);
	glUniform1f(scaleY_loc, scaleVartex_Y);
	
	
	//glDrawArrays(GL_TRIANGLES, 0, 18);
	//glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
	//glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(GLushort) * 1));
	//glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0); // Last one is the offet; its start from begining.
	
//	glDrawElementsInstanced(GL_TRIANGLES , 18, GL_UNSIGNED_SHORT,0,7); // 0 is first offset and 7 is seven instance 


	glutSwapBuffers();

}

/*******************************************/
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	//printf("MAx %f:", pSizeMax);
	glutCreateWindow("Instanced Hexagon");
	

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);// for keyboard call back function
	glutMainLoop();
}

/**********************************************/



