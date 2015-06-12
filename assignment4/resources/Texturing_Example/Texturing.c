/******************************************************************
*
* Texturing.c
*
* Description: This file demonstrates basic texturing of two
* triangles in shader-based OpenGL. The texture is provided as
* a BMP file and loaded during runtime. UV coordinates are
* given directly in the code. There is no lighting, projection,
* etc.
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

/* OpenGL includes */
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

/* Local includes */
#include "LoadShader.h"   /* Provides loading function for shader code */
#include "LoadTexture.h"  /* Provides loading function for BMP texture */

/*----------------------------------------------------------------*/

/* Handle to vertex buffer object */
GLuint VBO;
GLuint VAO;

GLuint ShaderProgram;

/* Variables for texture handling */
GLuint TextureID;
GLuint TextureUniform;
TextureDataPtr Texture;

/* Indices to vertex attributes; in this case positon and texture coordinates */
enum DataID {vPosition = 0, vUV = 1};

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;


const GLuint numVertices = 6;

/* Structure containing XYZ position and RGB color data */
typedef struct
{
    GLfloat Position[3];
    GLfloat UV[2];
} VertexData;

/* Define vertex positions and UV coordinates */
VertexData Vertices[] =
{
    {{-0.95, -0.90, 0.0}, {0.0, 0.0}},   /* Triangle 1 */
    {{ 0.90,  0.95, 0.0}, {1.0, 1.0}},
    {{-0.95,  0.95, 0.0}, {0.0, 1.0}},
    {{-0.90, -0.95, 0.0}, {0.0, 0.0}},   /* Triangle 2 */
    {{ 0.95, -0.95, 0.0}, {1.0, 0.0}},
    {{ 0.95,  0.90, 0.0}, {1.0, 1.0}}
};


/*----------------------------------------------------------------*/


/******************************************************************
*
* Display
*
* This function is called when the content of the window needs to be
* drawn/redrawn. It has been specified through 'glutDisplayFunc()';
* Enable vertex attributes, create binding between C program and
* attribute name in shader
*
*******************************************************************/

void Display()
{
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Make (previously created) buffer active and bind to target */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    /* Activate first (and only) texture unit */
    glActiveTexture(GL_TEXTURE0);

    /* Bind current texture  */
    glBindTexture(GL_TEXTURE_2D, TextureID);

    /* Get texture uniform handle from fragment shader */
    TextureUniform  = glGetUniformLocation(ShaderProgram, "myTextureSampler");

    /* Set location of uniform sampler variable */
    glUniform1i(TextureUniform, 0);

    /* Enable position and UV attribute */
    glEnableVertexAttribArray(vPosition);
    glEnableVertexAttribArray(vUV);

    /* For each vertex attribue specify location of data */
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
    glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData),
			  (const GLvoid*) sizeof(Vertices[0].Position));

    /* Issue draw command; use triangles as primitives */
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    /* Disable attributes */
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vUV);

    /* Swap between front and back buffer */
    glutSwapBuffers();
}


/******************************************************************
*
* SetupVertexBuffer
*
* Create vertex buffer object and load vertex data into buffer
*
*******************************************************************/

void SetupVertexBuffer()
{
    /* Generate one buffer name and store in handle */
    glGenBuffers(1, &VBO);

    /* Create new buffer object and assign name */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    /* Load data into buffer object */
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
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
* SetupTexture
*
* This function is called to load the texture and initialize
* texturing parameters
*
*******************************************************************/

void SetupTexture(void)
{
    /* Allocate texture container */
    Texture = malloc(sizeof(TextureDataPtr));

    int success = LoadTexture("data/uvtemplate.bmp", Texture);
    if (!success)
    {
        printf("Error loading texture. Exiting.\n");
	      exit(-1);
    }

    /* Create texture name and store in handle */
    glGenTextures(1, &TextureID);

    /* Bind texture */
    glBindTexture(GL_TEXTURE_2D, TextureID);

    /* Load texture image into memory */
    glTexImage2D(GL_TEXTURE_2D,     /* Target texture */
		 0,                 /* Base level */
		 GL_RGB,            /* Each element is RGB triple */
		 Texture->width,    /* Texture dimensions */
            Texture->height,
		 0,                 /* Border should be zero */
		 GL_BGR,            /* Data storage format for BMP file */
		 GL_UNSIGNED_BYTE,  /* Type of pixel data, one byte per channel */
		 Texture->data);    /* Pointer to image data  */

    /* Next set up texturing parameters */

    /* Repeat texture on edges when tiling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* Linear interpolation for magnification */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Trilinear MIP mapping for minification */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* Note: MIP mapping not visible due to fixed, i.e. static camera */
}

/******************************************************************
*
* Initialize
*
* This function is called to initialize rendering elements, setup
* vertex buffer objects, and to setup the vertex and fragment shader
*
*******************************************************************/

void Initialize(void)
{
    /* Set background (clear) color to black */
    glClearColor(0.0, 0.0, 0.0, 0.0);

    /* Setup Vertex buffer object */
    SetupVertexBuffer();

    /* Setup shaders and shader program */
    CreateShaderProgram();

    /* Setup texture */
    SetupTexture();
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 400);
    glutCreateWindow("CG Proseminar - Basic Textured Triangles");

    /* Initialize GL extension wrangler */
    #ifndef __APPLE__
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }
    #endif

    Initialize();

    /* Specify callback function for drawing/redrawing window content;
     * enter GLUT event processing loop, handing control over to GLUT */
    glutDisplayFunc(Display);
    glutMainLoop();

    /* ISO C requires main to return int */
    return 0;
}
