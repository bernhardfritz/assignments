/******************************************************************
*
* Interaction.c
*
* Description: This file demonstrates the loading of external
* triangle meshes provided in OBJ format. In addition, user
* interaction via mouse and keyboard is employed.
*
* The loaded triangle mesh is draw in wireframe mode and rotated
* around a reference axis. The user can control the rotation axis
* via the mouse and start/stop/reset animation via the keyboard.
*
* Computer Graphics Proseminar SS 2015
*
* Interactive Graphics and Simulation Group
* Institute of Computer Science
* University of Innsbruck
*
*******************************************************************/


/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* OpenGL includes */
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

/* Local includes */
#include "LoadShader.h"    /* Loading function for shader code */
#include "Matrix.h"        /* Functions for matrix handling */
#include "OBJParser.h"     /* Loading function for triangle meshes in OBJ format */


/*----------------------------------------------------------------*/

/* Flag for starting/stopping animation */
GLboolean anim = GL_TRUE;

/* Define handles to two vertex buffer objects */
GLuint VBO1, VBO2;

GLuint VAO;

/* Define handles to two index buffer objects */
GLuint IBO1, IBO2;

/* Indices to vertex attributes; in this case positon only */
enum DataID {vPosition = 0};

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;


/* Matrices for uniform variables in vertex shader */
float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16];       /* Camera view matrix */
float ModelMatrix[16];      /* Model matrix */

/* Transformation matrices for model rotation */
float RotationMatrixAnimX[16];
float RotationMatrixAnimY[16];
float RotationMatrixAnimZ[16];
float RotationMatrixAnim[16];

/* Variables for storing current rotation angles */
float angleX, angleY, angleZ = 0.0f;

/* Indices to active rotation axes */
enum {Xaxis=0, Yaxis=1, Zaxis=2};
int axis = Yaxis;

/* Indices to active triangle mesh */
enum {Model1=0, Model2=1};
int model = Model1;

/* Arrays for holding vertex data of the two models */
GLfloat *vertex_buffer_data1, *vertex_buffer_data2;

/* Arrays for holding indices of the two models */
GLushort *index_buffer_data1,  *index_buffer_data2;

/* Structures for loading of OBJ data */
obj_scene_data data1, data2;

/* Reference time for animation */
int oldTime = 0;


/*----------------------------------------------------------------*/



/******************************************************************
*
* Display
*
* This function is called when the content of the window needs to be
* drawn/redrawn. It has been specified through 'glutDisplayFunc()';
* Enable vertex attributes, create binding between C program and
* attribute name in shader, provide data for uniform variables
*
*******************************************************************/

void Display()
{
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(vPosition);

    /* Bind buffer with vertex data of currently active object */
    if(model == Model1)
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    else if(model == Model2)
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

   /* Bind buffer with index data of currently active object */
    if(model == Model1)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO1);
    else if(model == Model2)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);

    GLint size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    /* Associate program with uniform shader matrices */
    GLint projectionUniform = glGetUniformLocation(ShaderProgram, "ProjectionMatrix");
    if (projectionUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
	exit(-1);
    }
    glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, ProjectionMatrix);

    GLint ViewUniform = glGetUniformLocation(ShaderProgram, "ViewMatrix");
    if (ViewUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform ViewMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, ViewMatrix);

    GLint RotationUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (RotationUniform == -1)
    {
        fprintf(stderr, "Could not bind uniform ModelMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);

    /* Set state to only draw wireframe (no lighting used, yet) */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    /* Disable attributes */
    glDisableVertexAttribArray(vPosition);

    /* Swap between front and back buffer */
    glutSwapBuffers();
}


/******************************************************************
*
* Mouse
*
* Function is called on mouse button press; has been seta
* with glutMouseFunc(), x and y specify mouse coordinates,
* but are not used here.
*
*******************************************************************/

void Mouse(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
      /* Depending on button pressed, set rotation axis,
       * turn on animation */
        switch(button)
	{
	    case GLUT_LEFT_BUTTON:
	        axis = Xaxis;
		break;

	    case GLUT_MIDDLE_BUTTON:
  	        axis = Yaxis;
	        break;

	    case GLUT_RIGHT_BUTTON:
	        axis = Zaxis;
		break;
	}
	anim = GL_TRUE;
    }
}


/******************************************************************
*
* Keyboard
*
* Function to be called on key press in window; set by
* glutKeyboardFunc(); x and y specify mouse position on keypress;
* not used in this example
*
*******************************************************************/

void Keyboard(unsigned char key, int x, int y)
{
    switch( key )
    {
	/* Activate model one or two */
	case '1':
		model = Model1;
		break;

	case '2':
		model = Model2;
		break;

	/* Toggle animation */
	case '0':
		if (anim)
			anim = GL_FALSE;
		else
			anim = GL_TRUE;
		break;

	/* Reset initial rotation of object */
	case 'o':
	    SetIdentityMatrix(RotationMatrixAnimX);
	    SetIdentityMatrix(RotationMatrixAnimY);
	    SetIdentityMatrix(RotationMatrixAnimZ);
	    angleX = 0.0;
	    angleY = 0.0;
	    angleZ = 0.0;
	    break;

	case 'q': case 'Q':
	    exit(0);
		break;
    }

    glutPostRedisplay();
}


/******************************************************************
*
* OnIdle
*
* Function executed when no other events are processed; set by
* call to glutIdleFunc(); holds code for animation
*
*******************************************************************/

void OnIdle()
{
    /* Determine delta time between two frames to ensure constant animation */
    int newTime = glutGet(GLUT_ELAPSED_TIME);
    int delta = newTime - oldTime;
    oldTime = newTime;

    if(anim)
    {
        /* Increment rotation angles and update matrix */
        if(axis == Xaxis)
	{
  	    angleX = fmod(angleX + delta/20.0, 360.0);
	    SetRotationX(angleX, RotationMatrixAnimX);
	}
	else if(axis == Yaxis)
	{
	    angleY = fmod(angleY + delta/20.0, 360.0);
	    SetRotationY(angleY, RotationMatrixAnimY);
	}
	else if(axis == Zaxis)
	{
	    angleZ = fmod(angleZ + delta/20.0, 360.0);
	    SetRotationZ(angleZ, RotationMatrixAnimZ);
	}
    }

    /* Update of transformation matrices
     * Note order of transformations and rotation of reference axes */
    MultiplyMatrix(RotationMatrixAnimX, RotationMatrixAnimY, RotationMatrixAnim);
    MultiplyMatrix(RotationMatrixAnim, RotationMatrixAnimZ, ModelMatrix);

    /* Issue display refresh */
    glutPostRedisplay();
}


/******************************************************************
*
* SetupDataBuffers
*
* Create buffer objects and load data into buffers
*
*******************************************************************/

void SetupDataBuffers()
{
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, data1.vertex_count*3*sizeof(GLfloat), vertex_buffer_data1, GL_STATIC_DRAW);

    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, data2.vertex_count*3*sizeof(GLfloat), vertex_buffer_data2, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data1.face_count*3*sizeof(GLushort), index_buffer_data1, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data2.face_count*3*sizeof(GLushort), index_buffer_data2, GL_STATIC_DRAW);
}


/******************************************************************
*
* AddShader
*
* This function creates and adds individual shaders
*
*******************************************************************/

void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType)
{
    /* Create shader object */
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0)
    {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    /* Associate shader source code string with shader object */
    glShaderSource(ShaderObj, 1, &ShaderCode, NULL);

    GLint success = 0;
    GLchar InfoLog[1024];

    /* Compile shader source code */
    glCompileShader(ShaderObj);
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    /* Associate shader with shader program */
    glAttachShader(ShaderProgram, ShaderObj);
}


/******************************************************************
*
* CreateShaderProgram
*
* This function creates the shader program; vertex and fragment
* shaders are loaded and linked into program; final shader program
* is put into the rendering pipeline
*
*******************************************************************/

void CreateShaderProgram()
{
    /* Allocate shader object */
    ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0)
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    /* Load shader code from file */
    VertexShaderString = LoadShader("shaders/vertexshader.vs");
    FragmentShaderString = LoadShader("shaders/fragmentshader.fs");

    /* Separately add vertex and fragment shader to program */
    AddShader(ShaderProgram, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, FragmentShaderString, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024];

    /* Link shader code into executable shader program */
    glLinkProgram(ShaderProgram);

    /* Check results of linking step */
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

    if (Success == 0)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Check if shader program can be executed */
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);

    if (!Success)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Put linked shader program into drawing pipeline */
    glUseProgram(ShaderProgram);
}


/******************************************************************
*
* Initialize
*
* This function is called to initialize rendering elements, setup
* vertex buffer objects, and to setup the vertex and fragment shader;
* meshes are loaded from files in OBJ format; data is copied from
* structures into vertex and index arrays
*
*******************************************************************/

void Initialize()
{
    int i;
    int success;

    /* Load first OBJ model */
    char* filename1 = "models/teapot.obj";
    success = parse_obj_scene(&data1, filename1);

    if(!success)
        printf("Could not load file. Exiting.\n");

    /* Load second OBJ model */
    char* filename2 = "models/suzanne.obj";
    success = parse_obj_scene(&data2, filename2);

    if(!success)
        printf("Could not load file. Exiting.\n");

    /*  Copy mesh data from structs into appropriate arrays */
    int vert = data1.vertex_count;
    int indx = data1.face_count;

    vertex_buffer_data1 = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    index_buffer_data1 = (GLushort*) calloc (indx*3, sizeof(GLushort));

    /* Vertices */
    for(i=0; i<vert; i++)
    {
        vertex_buffer_data1[i*3] = (GLfloat)(*data1.vertex_list[i]).e[0];
	vertex_buffer_data1[i*3+1] = (GLfloat)(*data1.vertex_list[i]).e[1];
	vertex_buffer_data1[i*3+2] = (GLfloat)(*data1.vertex_list[i]).e[2];
    }

    /* Indices */
    for(i=0; i<indx; i++)
    {
	index_buffer_data1[i*3] = (GLushort)(*data1.face_list[i]).vertex_index[0];
	index_buffer_data1[i*3+1] = (GLushort)(*data1.face_list[i]).vertex_index[1];
	index_buffer_data1[i*3+2] = (GLushort)(*data1.face_list[i]).vertex_index[2];
    }

    vert = data2.vertex_count;
    indx = data2.face_count;

    vertex_buffer_data2 = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
    index_buffer_data2 = (GLushort*) calloc (indx*3, sizeof(GLushort));

    /* Vertices */
    for(i=0; i<vert; i++)
    {
	vertex_buffer_data2[i*3] = (GLfloat)(*data2.vertex_list[i]).e[0];
	vertex_buffer_data2[i*3+1] = (GLfloat)(*data2.vertex_list[i]).e[1];
	vertex_buffer_data2[i*3+2] = (GLfloat)(*data2.vertex_list[i]).e[2];
    }

    /* Indices */
    for(i=0; i<indx; i++)
    {
	index_buffer_data2[i*3] = (GLushort)(*data2.face_list[i]).vertex_index[0];
	index_buffer_data2[i*3+1] = (GLushort)(*data2.face_list[i]).vertex_index[1];
	index_buffer_data2[i*3+2] = (GLushort)(*data2.face_list[i]).vertex_index[2];
    }

    /* Set background (clear) color to blue */
    glClearColor(0.0, 0.0, 0.4, 0.0);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Setup vertex, color, and index buffer objects */
    SetupDataBuffers();

    /* Setup shaders and shader program */
    CreateShaderProgram();

    /* Initialize matrices */
    SetIdentityMatrix(ProjectionMatrix);
    SetIdentityMatrix(ViewMatrix);
    SetIdentityMatrix(ModelMatrix);

    /* Initialize animation matrices */
    SetIdentityMatrix(RotationMatrixAnimX);
    SetIdentityMatrix(RotationMatrixAnimY);
    SetIdentityMatrix(RotationMatrixAnimZ);
    SetIdentityMatrix(RotationMatrixAnim);

    /* Set projection transform */
    float fovy = 45.0;
    float aspect = 1.0;
    float nearPlane = 1.0;
    float farPlane = 50.0;
    SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);

    /* Set viewing transform */
    float camera_disp = -10.0;
    SetTranslation(0.0, 0.0, camera_disp, ViewMatrix);
}


/******************************************************************
*
* main
*
* Main function to setup GLUT, GLEW, and enter rendering loop
*
*******************************************************************/

int main(int argc, char** argv)
{
    /* Initialize GLUT; set double buffered window and RGBA color model */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 400);
    glutCreateWindow("CG Proseminar - User Interaction");

    /* Initialize GL extension wrangler */
    #ifndef __APPLE__
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }
    #endif

    /* Setup scene and rendering parameters */
    Initialize();

    /* Specify callback functions;enter GLUT event processing loop,
     * handing control over to GLUT */
    glutIdleFunc(OnIdle);
    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);

    glutMainLoop();

    /* ISO C requires main to return int */
    return 0;
}
