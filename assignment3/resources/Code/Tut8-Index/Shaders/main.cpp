#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>

#define USING_INDEX_BUFFER 1

#ifdef USING_INDEX_BUFFER
	#define NUM_VERTICES 6
	#define NUM_INDICES 9	
#else
	#define NUM_VERTICES 9
#endif


#define BUFFER_OFFSET(i) ((char *)NULL + (i))
GLuint shaderProgramID;
GLuint vao = 0;
GLuint vbo;
GLuint positionID, colorID;
GLuint indexBufferID;

#pragma region SHADER_FUNCTIONS
static char* readFile(const char* filename) {
	// Open the file
	FILE* fp = fopen (filename, "r");
	// Move the file pointer to the end of the file and determing the length
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* contents = new char[file_length+1];
	// zero out memory
	for (int i = 0; i < file_length+1; i++) {
		contents[i] = 0;
	}
	// Here's the actual read
	fread (contents, 1, file_length, fp);
	// This is how you denote the end of a string in C
	contents[file_length+1] = '\0';
	fclose(fp);
	return contents;
}

bool compiledStatus(GLint shaderID){
	GLint compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled) {
		return true;
	}
	else {
		GLint logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char* msgBuffer = new char[logLength];
		glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer);
		printf ("%s\n", msgBuffer);
		delete (msgBuffer);
		return false;
	}
}

GLuint makeVertexShader(const char* shaderSource) {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource (vertexShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(vertexShaderID);
	bool compiledCorrectly = compiledStatus(vertexShaderID);
	if (compiledCorrectly) {
		return vertexShaderID;
	}
	return -1;
}

GLuint makeFragmentShader(const char* shaderSource) {
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(fragmentShaderID);
	bool compiledCorrectly = compiledStatus(fragmentShaderID);
	if (compiledCorrectly) {
		return fragmentShaderID;
	}
	return -1;
}

GLuint makeShaderProgram (GLuint vertexShaderID, GLuint fragmentShaderID) {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	glLinkProgram(shaderID);
	return shaderID;
}
#pragma endregion SHADER_FUNCTIONS

// Any time the window is resized, this function gets called.  It's setup to the 
// "glutReshapeFunc" in main.
void changeViewport(int w, int h){
	glViewport(0, 0, w, h);
}

// Here is the function that gets called each time the window needs to be redrawn.
// It is the "paint" method for our program, and is set up from the glutDisplayFunc in main
void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#ifdef USING_INDEX_BUFFER
	glDrawElements (GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL);
#else
	glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
#endif
	glutSwapBuffers();
}

int main (int argc, char** argv) {
	// Standard stuff...
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Index Buffers");
	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glewInit();

#ifdef USING_INDEX_BUFFER
	GLfloat vertices[] = {0.0f, 0.5f, 0.0f, // 0
						-0.25f, 0.0f, 0.0f, // 1
						0.25f, 0.0f, 0.0f, // 2
						-0.5f, -0.5f, 0.0f, // 3
						0.0f, -0.5f, 0.0f, // 4
						0.5f, -0.5f, 0.0f // 5
	};
	GLfloat colors[] = {1.0f, 0.00f, 0.0f, 1.0f, // 0
						0.0f, 1.00f, 0.0f, 1.0f, // 1
						0.0f, 0.00f, 1.0f, 1.0f, // 2
						0.0f, 0.00f, 1.0f, 1.0f, // 3
						1.0f, 0.00f, 0.0f, 1.0f, // 4
						0.0f, 1.0f, 0.0f, 1.0f // 5
	};
	
	GLuint indices[] = {0, 1, 2, 1, 3, 4, 2, 4, 5};
#else
	GLfloat vertices[] = {-0.5f, -0.5f, 0.0f, 
						0.0f, -0.5f, 0.0f, 
						-0.25f, 0.0f, 0.0f,
						-0.25f, 0.0f, 0.0f,
						0.25f, 0.0f, 0.0f,
						0.0f, 0.5f, 0.0f,
						0.0f, -0.5f, 0.0f,
						0.5f, -0.5f, 0.0f,
						0.25f, 0.0f, 0.0f
	};
	GLfloat colors[] = {1.0f, 0.00f, 0.0f, 1.0f, 
						0.0f, 1.00f, 0.0f, 1.0f,
						0.0f, 0.0f, 1.0f, 1.0f,
						1.0f, 0.00f, 0.0f, 1.0f, 
						0.0f, 1.00f, 0.0f, 1.0f,
						0.0f, 0.0f, 1.0f, 1.0f,
						1.0f, 0.00f, 0.0f, 1.0f, 
						0.0f, 1.00f, 0.0f, 1.0f,
						0.0f, 0.0f, 1.0f, 1.0f
	};
	
#endif

	// Make a shader
	char* vertexShaderSourceCode = readFile("vertexShader.vsh");
	char* fragmentShaderSourceCode = readFile("fragmentShader.fsh");
	GLuint vertShaderID = makeVertexShader(vertexShaderSourceCode);
	GLuint fragShaderID = makeFragmentShader(fragmentShaderSourceCode);
	shaderProgramID = makeShaderProgram(vertShaderID, fragShaderID);

	// Create the "remember all"
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Create the buffer, but don't load anything yet
	glBufferData(GL_ARRAY_BUFFER, 7*NUM_VERTICES*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	// Load the vertex points
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3*NUM_VERTICES*sizeof(GLfloat), vertices); 
	// Load the colors right after that
	glBufferSubData(GL_ARRAY_BUFFER, 3*NUM_VERTICES*sizeof(GLfloat),4*NUM_VERTICES*sizeof(GLfloat), colors);

#ifdef USING_INDEX_BUFFER
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES*sizeof(GLuint), indices, GL_STATIC_DRAW);
#endif
	
	// Find the position of the variables in the shader
	positionID = glGetAttribLocation(shaderProgramID, "s_vPosition");
	colorID = glGetAttribLocation(shaderProgramID, "s_vColor");

	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));
	glUseProgram(shaderProgramID);
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);
	
	glutMainLoop();
	
	return 0;
}