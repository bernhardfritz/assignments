#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include "MathHelper.h"		// NEW!
#include "phlegm_small.h"

#define USING_INDEX_BUFFER 1

#ifdef USING_INDEX_BUFFER
	#define NUM_VERTICES num_vertices
	#define NUM_INDICES num_indices	
#else
	#define NUM_VERTICES num_vertices
#endif

// From http://www.opengl.org/registry/specs/EXT/pixel_buffer_object.txt 
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


GLuint shaderProgramID;
GLuint vao = 0;
GLuint vbo;
GLuint positionID, colorID;
GLuint indexBufferID; 

// ==================== BEGIN NEW STUFF ============================
GLuint	perspectiveMatrixID, viewMatrixID, modelMatrixID;	// IDs of variables mP, mV and mM in the shader

GLfloat* rotXMatrix;	// Matrix for rotations about the X axis
GLfloat* rotYMatrix;	// Matrix for rotations about the Y axis
GLfloat* rotZMatrix;	// Matrix for rotations about the Z axis
GLfloat* transMatrix;	// Matrix for changing the position of the object
GLfloat* scaleMatrix;	// Duh..
GLfloat* tempMatrix1;	// A temporary matrix for holding intermediate multiplications
GLfloat* M;				// The final model matrix M to change into world coordinates

GLfloat* V;				// The camera matrix (for position/rotation) to change into camera coordinates
GLfloat* P;				// The perspective matrix for the camera (to give the scene depth); initialize this ONLY ONCE!

GLfloat  theta;			// An amount of rotation along one axis
GLfloat	 scaleAmount;	// In case the object is too big or small


void initMatrices() {

	theta = 0.0f;
	scaleAmount = 1.0f;

	// Allocate memory for the matrices and initialize them to the Identity matrix
	rotXMatrix = new GLfloat[16];	MathHelper::makeIdentity(rotXMatrix);
	rotYMatrix = new GLfloat[16];	MathHelper::makeIdentity(rotYMatrix);
	rotZMatrix = new GLfloat[16];	MathHelper::makeIdentity(rotZMatrix);
	transMatrix = new GLfloat[16];	MathHelper::makeIdentity(transMatrix);
	scaleMatrix = new GLfloat[16];	MathHelper::makeIdentity(scaleMatrix);
	tempMatrix1 = new GLfloat[16];	MathHelper::makeIdentity(tempMatrix1);
	M = new GLfloat[16];			MathHelper::makeIdentity(M);
	V = new GLfloat[16];			MathHelper::makeIdentity(V);
	P = new GLfloat[16];			MathHelper::makeIdentity(P);

	// Set up the (P)erspective matrix only once! Arguments are 1) the resulting matrix, 2) FoV, 3) aspect ratio, 4) near plane 5) far plane
	MathHelper::makePerspectiveMatrix(P, 60.0f, 1.0f, 1.0f, 1000.0f);
}
//==================== END NEW STUFF ==========================================


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

	// New!
	glUseProgram(shaderProgramID);
	theta+= 0.01f;
	scaleAmount = sin(theta);

	// Fill the matrices with valid data
	MathHelper::makeScale(scaleMatrix, 0.1f, 0.1f, 0.1f);			// Fill the scaleMatrix variable
	MathHelper::makeRotateY(rotYMatrix, theta);						// Fill the rotYMatrix variable
	MathHelper::makeTranslate(transMatrix, 0.0f, -0.25f, -2.0f);		// Fill the transMatrix to push the model back 1 "unit" into the scene
	// MathHelper::makeTranslate(V, 0.0f, 1.0f, 0.0f);			// Fill the (V)iew matrix to push the world up 1 unit
	
	// Multiply them together 
	//MathHelper::matrixMult4x4(M, transMatrix, rotYMatrix);	// Order is important!
	//MathHelper::matrixMult4x4(M, rotYMatrix, transMatrix);	// This is backwards, unless you want orbiting!

	MathHelper::matrixMult4x4(tempMatrix1, rotYMatrix, scaleMatrix);	// Scale, then rotate...
	MathHelper::matrixMult4x4(M, transMatrix, tempMatrix1);	// ... then multiply THAT by the translate
		
	// Important! Pass that data to the shader variables
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, M);
	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, V);
	glUniformMatrix4fv(perspectiveMatrixID, 1, GL_TRUE, P);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#ifdef USING_INDEX_BUFFER
	glDrawElements (GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL);
#else
	glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
#endif
	glutSwapBuffers();
	glutPostRedisplay();		// NEW! This calls "render" again, allowing for animation!
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

	initMatrices(); // New <========================================

#ifdef USING_INDEX_BUFFER

/*
	GLfloat colors[] = {
	0.333f, 0.333f, 0.333f, 1.0f,
	0.75f,	0.75f,	0.0f,	1.0f,
	1.0f,	0.25f,	0.25f,	1.0f,
	1.0f,	0.0f,	0.0f,	1.0f,
	1.0f,	0.25f,	0.25f,	1.0f,
	0.75f,	0.75f,	0.0f,	1.0f,
	0.5f,	0.5f,	0.5f,	1.0f,
	0.0f,	0.0f,	0.0f,	1.0f
};
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
	*/
	
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
	//glBufferData(GL_ARRAY_BUFFER, 7*NUM_VERTICES*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 6*NUM_VERTICES*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	
	// Load the vertex points
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3*NUM_VERTICES*sizeof(GLfloat), vertices);
	// Load the colors right after that
	//glBufferSubData(GL_ARRAY_BUFFER, 3*NUM_VERTICES*sizeof(GLfloat),4*NUM_VERTICES*sizeof(GLfloat), colors);
	glBufferSubData(GL_ARRAY_BUFFER, 3*NUM_VERTICES*sizeof(GLfloat),3*NUM_VERTICES*sizeof(GLfloat), normals);
	
	
#ifdef USING_INDEX_BUFFER
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES*sizeof(GLuint), indices, GL_STATIC_DRAW);
#endif
	
	// Find the position of the variables in the shader
	positionID = glGetAttribLocation(shaderProgramID, "s_vPosition");
	colorID = glGetAttribLocation(shaderProgramID, "s_vColor");
	
	// ============ New! glUniformLocation is how you pull IDs for uniform variables===============
	perspectiveMatrixID = glGetUniformLocation(shaderProgramID, "mP");
	viewMatrixID = glGetUniformLocation(shaderProgramID, "mV");
	modelMatrixID = glGetUniformLocation(shaderProgramID, "mM");
	//=============================================================================================

	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));
	glVertexAttribPointer(colorID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));
	printf ("BUFFER_OFFSET is: %d\n", sizeof(vertices));

	glUseProgram(shaderProgramID);
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);
	
	glutMainLoop();
	
	return 0;
}