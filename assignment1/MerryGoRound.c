/******************************************************************
*
* MerryGoRound.c
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
#include "LoadShader.h"   /* Provides loading function for shader code */
#include "Matrix.h"
#include "ObjLoader.h"


/*----------------------------------------------------------------*/

/* Define handle to a vertex buffer object */
GLuint* VBO;

/* Define handle to a vertex array object */
GLuint* VAO;

/* Define handle to a color buffer object */
GLuint* CBO;

/* Define handle to an index buffer object */
GLuint* IBO;

GLfloat** vertex_buffer_data = 0;
GLushort** index_buffer_data = 0;
GLfloat** color_buffer_data = 0;

int objects = 0;
int* vertices = 0;
int* faces = 0;

/* Indices to vertex attributes; in this case positon and color */
enum DataID {vPosition = 0, vColor = 1};

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */
float** ModelMatrix; /* Model matrix */

/* Transformation matrices for initial position */
float** TranslateOrigin;
float TranslateDown[16];
float** RotateX;
float** RotateY;
float** RotateZ;
float** InitialTransform;
float** UniformScale;

void generateOctagonalPrismVertexBuffer(GLfloat* result, GLfloat radius, GLfloat height) {
  GLfloat temp[54] = {
    0.0               , 0.0               , -height/2.0,
    radius            , 0.0               , -height/2.0,
    radius/sqrtf(2.0) , radius/sqrtf(2.0) , -height/2.0,
    0.0               , radius            , -height/2.0,
    -radius/sqrtf(2.0), radius/sqrtf(2.0) , -height/2.0,
    -radius           , 0.0               , -height/2.0,
    -radius/sqrtf(2.0), -radius/sqrtf(2.0), -height/2.0,
    0.0               , -radius           , -height/2.0,
    radius/sqrtf(2.0) , -radius/sqrtf(2.0), -height/2.0,
    0.0               , 0.0               , height/2.0,
    radius            , 0.0               , height/2.0,
    radius/sqrtf(2.0) , radius/sqrtf(2.0) , height/2.0,
    0.0               , radius            , height/2.0,
    -radius/sqrtf(2.0), radius/sqrtf(2.0) , height/2.0,
    -radius           , 0.0               , height/2.0,
    -radius/sqrtf(2.0), -radius/sqrtf(2.0), height/2.0,
    0.0               , -radius           , height/2.0,
    radius/sqrtf(2.0) , -radius/sqrtf(2.0), height/2.0
  };

  memcpy(result, temp, 54*sizeof(GLfloat));
}

void generateOctagonalPrismColorBuffer(GLfloat* result) {
  GLfloat temp[54] = {
      0.0, 0.0, 1.0,
      1.0, 0.0, 1.0,
      1.0, 1.0, 1.0,
      0.0, 1.0, 1.0,
      0.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 1.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 1.0, 1.0,
      0.0, 0.0, 1.0,
      1.0, 0.0, 1.0,
      1.0, 1.0, 1.0,
      0.0, 1.0, 1.0,
      0.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 1.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 1.0, 1.0
  };

  memcpy(result, temp, 54*sizeof(GLfloat));
}

void generateOctagonalPrismIndexBuffer(GLushort* result) {
  GLushort temp[96] = {
      0, 1 , 2 ,
      0, 2 , 3 ,
      0, 3 , 4 ,
      0, 4 , 5 ,
      0, 5 , 6 ,
      0, 6 , 7 ,
      0, 7 , 8 ,
      0, 8 , 1 ,
      9, 10, 11,
      9, 11, 12,
      9, 12, 13,
      9, 13, 14,
      9, 14, 15,
      9, 15, 16,
      9, 16, 17,
      9, 17, 10,
      1,  2, 11,
      2,  3, 12,
      3,  4, 13,
      4,  5, 14,
      5,  6, 15,
      6,  7, 16,
      7,  8, 17,
      8,  1, 10,
      10, 11, 1,
      11, 12, 2,
      12, 13, 3,
      13, 14, 4,
      14, 15, 5,
      15, 16, 6,
      16, 17, 7,
      17, 10, 8
  };

  memcpy(result, temp, 96*sizeof(GLushort));
}

void generateOctagonalPrism(GLfloat* result_vertex_buffer_data, GLushort* result_index_buffer_data, GLfloat* result_color_buffer_data,GLfloat radius, GLfloat height) {
  generateOctagonalPrismVertexBuffer(result_vertex_buffer_data, radius, height);
  generateOctagonalPrismIndexBuffer(result_index_buffer_data);
  generateOctagonalPrismColorBuffer(result_color_buffer_data);
}

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(int i=0; i<objects; i++) {
      glEnableVertexAttribArray(vPosition);
      glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
      glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

      glEnableVertexAttribArray(vColor);
      glBindBuffer(GL_ARRAY_BUFFER, CBO[i]);
      glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[i]);
      GLint size;
      glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

      /* Associate program with shader matrices */
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
      glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix[i]);

      /* Issue draw command, using indexed triangle list */
      glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

      /* Disable attributes */
      glDisableVertexAttribArray(vPosition);
      glDisableVertexAttribArray(vColor);
    }
    /* Swap between front and back buffer */
    glutSwapBuffers();
}


/******************************************************************
*
* OnIdle
*
*
*
*******************************************************************/

void OnIdle()
{
    float angle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (180.0/M_PI);
    float RotationMatrixAnim[16];

    /* Time dependent rotation */
    SetRotationY(angle, RotationMatrixAnim);

    /* Time dependent translation */

    /* Apply model rotation */
    for(int i=0; i<objects; i++) {
      MultiplyMatrix(RotationMatrixAnim, InitialTransform[i], ModelMatrix[i]);
    }

    for(int i=6; i<objects; i++) {
      float distance = sinf((angle+(i-6)*90)/16)*4;
      float TranslationMatrixAnim[16];
      SetTranslation(0, distance, 0, TranslationMatrixAnim);
      MultiplyMatrix(TranslationMatrixAnim, ModelMatrix[i], ModelMatrix[i]);
    }

    /* Request redrawing forof window content */
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
    for(int i=0; i<objects; i++) {
      glGenBuffers(1, &VBO[i]);
      glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
      glBufferData(GL_ARRAY_BUFFER, vertices[i]*3*sizeof(GLfloat), vertex_buffer_data[i], GL_STATIC_DRAW);
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    for(int i=0; i<objects; i++) {
      glGenBuffers(1, &IBO[i]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[i]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces[i]*3*sizeof(GLushort), index_buffer_data[i], GL_STATIC_DRAW);
    }

    for(int i=0; i<objects; i++) {
      glGenBuffers(1, &CBO[i]);
      glBindBuffer(GL_ARRAY_BUFFER, CBO[i]);
      glBufferData(GL_ARRAY_BUFFER, vertices[i]*3*sizeof(GLfloat), color_buffer_data[i], GL_STATIC_DRAW);
    }
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
    VertexShaderString = LoadShader("vertexshader.vs");
    FragmentShaderString = LoadShader("fragmentshader.fs");

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
* vertex buffer objects, and to setup the vertex and fragment shader
*
*******************************************************************/

void Initialize()
{
    /* Set background (clear) color to black */
    glClearColor(0.0, 0.0, 0.0, 0.0);

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
    for(int i=0; i<objects; i++) {
      SetIdentityMatrix(ModelMatrix[i]);
    }

    /* Set projection transform */
    float fovy = 45.0;
    float aspect = 1.0;
    float nearPlane = 1.0;
    float farPlane = 100.0;
    SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);

    /* Set viewing transform */
    float camera_disp = -50.0;
    SetTranslation(0.0, 0.0, camera_disp, ViewMatrix);

    /* translate bottom plane */
    SetTranslation(0, 0, 10, TranslateOrigin[0]);
    SetRotationX(90, RotateX[0]);
    MultiplyMatrix(RotateX[0], TranslateOrigin[0], InitialTransform[0]);

    /* translate top plane */
    SetTranslation(0, 0, -10, TranslateOrigin[1]);
    SetRotationX(90, RotateX[1]);
    MultiplyMatrix(RotateX[1], TranslateOrigin[1], InitialTransform[1]);

    /* translate first column */
    SetTranslation(12.5, 0, 0, TranslateOrigin[2]);
    SetRotationX(90, RotateX[2]);
    MultiplyMatrix(RotateX[2], TranslateOrigin[2], InitialTransform[2]);

    /* translate second column */
    SetTranslation(-12.5, 0, 0, TranslateOrigin[3]);
    SetRotationX(90, RotateX[3]);
    MultiplyMatrix(RotateX[3], TranslateOrigin[3], InitialTransform[3]);

    /* translate third column */
    SetTranslation(0, 12.5, 0, TranslateOrigin[4]);
    SetRotationX(90, RotateX[4]);
    MultiplyMatrix(RotateX[4], TranslateOrigin[4], InitialTransform[4]);

    /* translate fourth column */
    SetTranslation(0, -12.5, 0, TranslateOrigin[5]);
    SetRotationX(90, RotateX[5]);
    MultiplyMatrix(RotateX[5], TranslateOrigin[5], InitialTransform[5]);

    /* rotate, translate and scale first teddy */
    SetTranslation(-50, 0, 0, TranslateOrigin[6]);
    SetRotationY(0, RotateY[6]);
    SetUniformScale(0.25, UniformScale[6]);
    MultiplyMatrix(RotateY[6], TranslateOrigin[6], InitialTransform[6]);
    MultiplyMatrix(UniformScale[6], InitialTransform[6], InitialTransform[6]);

    /* rotate, translate and scale second teddy */
    SetTranslation(-50, 0, 0, TranslateOrigin[7]);
    SetRotationY(90, RotateY[7]);
    SetUniformScale(0.25, UniformScale[7]);
    MultiplyMatrix(RotateY[7], TranslateOrigin[7], InitialTransform[7]);
    MultiplyMatrix(UniformScale[7], InitialTransform[7], InitialTransform[7]);

    /* rotate, translate and scale third teddy */
    SetTranslation(-50, 0, 0, TranslateOrigin[8]);
    SetRotationY(180, RotateY[8]);
    SetUniformScale(0.25, UniformScale[8]);
    MultiplyMatrix(RotateY[8], TranslateOrigin[8], InitialTransform[8]);
    MultiplyMatrix(UniformScale[8], InitialTransform[8], InitialTransform[8]);

    /* rotate, translate and scale fourth teddy */
    SetTranslation(-50, 0, 0, TranslateOrigin[9]);
    SetRotationY(270, RotateY[9]);
    SetUniformScale(0.25, UniformScale[9]);
    MultiplyMatrix(RotateY[9], TranslateOrigin[9], InitialTransform[9]);
    MultiplyMatrix(UniformScale[9], InitialTransform[9], InitialTransform[9]);
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
    objects = 10;
    VBO = malloc(objects*sizeof(GLint));
    IBO = malloc(objects*sizeof(GLint));
    CBO = malloc(objects*sizeof(GLint));
    vertices = malloc(objects*sizeof(int));
    faces = malloc(objects*sizeof(int));
    vertex_buffer_data = malloc(objects*sizeof(GLfloat*));
    index_buffer_data = malloc(objects*sizeof(GLushort*));
    color_buffer_data = malloc(objects*sizeof(GLfloat*));
    ModelMatrix = malloc(objects*sizeof(float*));
    TranslateOrigin = malloc(objects*sizeof(float*));
    RotateX = malloc(objects*sizeof(float*));
    RotateY = malloc(objects*sizeof(float*));
    RotateZ = malloc(objects*sizeof(float*));
    InitialTransform = malloc(objects*sizeof(float*));
    UniformScale = malloc(objects*sizeof(float*));

    vertices[0] = 18;
    faces[0] = 32;
    vertex_buffer_data[0] = malloc(vertices[0]*3*sizeof(GLfloat));
    index_buffer_data[0] = malloc(faces[0]*3*sizeof(GLushort));
    color_buffer_data[0] = malloc(vertices[0]*3*sizeof(GLfloat));
    ModelMatrix[0] = malloc(16*sizeof(float));
    TranslateOrigin[0] = malloc(16*sizeof(float));
    RotateX[0] = malloc(16*sizeof(float));
    RotateY[0] = malloc(16*sizeof(float));
    RotateZ[0] = malloc(16*sizeof(float));
    InitialTransform[0] = malloc(16*sizeof(float));
    UniformScale[0] = malloc(16*sizeof(float));
    generateOctagonalPrism(vertex_buffer_data[0],index_buffer_data[0],color_buffer_data[0],20.0,2.5);

    vertices[1] = 18;
    faces[1] = 32;
    vertex_buffer_data[1] = malloc(vertices[1]*3*sizeof(GLfloat));
    index_buffer_data[1] = malloc(faces[1]*3*sizeof(GLushort));
    color_buffer_data[1] = malloc(vertices[1]*3*sizeof(GLfloat));
    ModelMatrix[1] = malloc(16*sizeof(float));
    TranslateOrigin[1] = malloc(16*sizeof(float));
    RotateX[1] = malloc(16*sizeof(float));
    RotateY[1] = malloc(16*sizeof(float));
    RotateZ[1] = malloc(16*sizeof(float));
    InitialTransform[1] = malloc(16*sizeof(float));
    UniformScale[1] = malloc(16*sizeof(float));
    generateOctagonalPrism(vertex_buffer_data[1],index_buffer_data[1],color_buffer_data[1],20.0,2.5);

    vertices[2] = 18;
    faces[2] = 32;
    vertex_buffer_data[2] = malloc(vertices[2]*3*sizeof(GLfloat));
    index_buffer_data[2] = malloc(faces[2]*3*sizeof(GLushort));
    color_buffer_data[2] = malloc(vertices[2]*3*sizeof(GLfloat));
    ModelMatrix[2] = malloc(16*sizeof(float));
    TranslateOrigin[2] = malloc(16*sizeof(float));
    RotateX[2] = malloc(16*sizeof(float));
    RotateY[2] = malloc(16*sizeof(float));
    RotateZ[2] = malloc(16*sizeof(float));
    InitialTransform[2] = malloc(16*sizeof(float));
    UniformScale[2] = malloc(16*sizeof(float));
    generateOctagonalPrism(vertex_buffer_data[2],index_buffer_data[2],color_buffer_data[2],1.0,20);

    vertices[3] = 18;
    faces[3] = 32;
    vertex_buffer_data[3] = malloc(vertices[3]*3*sizeof(GLfloat));
    index_buffer_data[3] = malloc(faces[3]*3*sizeof(GLushort));
    color_buffer_data[3] = malloc(vertices[3]*3*sizeof(GLfloat));
    ModelMatrix[3] = malloc(16*sizeof(float));
    TranslateOrigin[3] = malloc(16*sizeof(float));
    RotateX[3] = malloc(16*sizeof(float));
    RotateY[3] = malloc(16*sizeof(float));
    RotateZ[3] = malloc(16*sizeof(float));
    InitialTransform[3] = malloc(16*sizeof(float));
    UniformScale[3] = malloc(16*sizeof(float));
    generateOctagonalPrism(vertex_buffer_data[3],index_buffer_data[3],color_buffer_data[3],1.0,20);

    vertices[4] = 18;
    faces[4] = 32;
    vertex_buffer_data[4] = malloc(vertices[4]*3*sizeof(GLfloat));
    index_buffer_data[4] = malloc(faces[4]*3*sizeof(GLushort));
    color_buffer_data[4] = malloc(vertices[4]*3*sizeof(GLfloat));
    ModelMatrix[4] = malloc(16*sizeof(float));
    TranslateOrigin[4] = malloc(16*sizeof(float));
    RotateX[4] = malloc(16*sizeof(float));
    RotateY[4] = malloc(16*sizeof(float));
    RotateZ[4] = malloc(16*sizeof(float));
    InitialTransform[4] = malloc(16*sizeof(float));
    UniformScale[4] = malloc(16*sizeof(float));
    generateOctagonalPrism(vertex_buffer_data[4],index_buffer_data[4],color_buffer_data[4],1.0,20);

    vertices[5] = 18;
    faces[5] = 32;
    vertex_buffer_data[5] = malloc(vertices[5]*3*sizeof(GLfloat));
    index_buffer_data[5] = malloc(faces[5]*3*sizeof(GLushort));
    color_buffer_data[5] = malloc(vertices[5]*3*sizeof(GLfloat));
    ModelMatrix[5] = malloc(16*sizeof(float));
    TranslateOrigin[5] = malloc(16*sizeof(float));
    RotateX[5] = malloc(16*sizeof(float));
    RotateY[5] = malloc(16*sizeof(float));
    RotateZ[5] = malloc(16*sizeof(float));
    InitialTransform[5] = malloc(16*sizeof(float));
    UniformScale[5] = malloc(16*sizeof(float));
    generateOctagonalPrism(vertex_buffer_data[5],index_buffer_data[5],color_buffer_data[5],1.0,20);

    char* fname="data/teddy.obj";
    vertices[6] = count('v',fname);
    faces[6] = count('f',fname);
    printf("%d %d\n",vertices[6],faces[6]);
    vertex_buffer_data[6] = malloc(vertices[6]*3*sizeof(GLfloat));
    index_buffer_data[6] = malloc(faces[6]*3*sizeof(GLushort));
    color_buffer_data[6] = malloc(vertices[6]*3*sizeof(GLfloat));
    ModelMatrix[6] = malloc(16*sizeof(float));
    TranslateOrigin[6] = malloc(16*sizeof(float));
    RotateX[6] = malloc(16*sizeof(float));
    RotateY[6] = malloc(16*sizeof(float));
    RotateZ[6] = malloc(16*sizeof(float));
    InitialTransform[6] = malloc(16*sizeof(float));
    UniformScale[6] = malloc(16*sizeof(float));
    loadObj(vertex_buffer_data[6], index_buffer_data[6], color_buffer_data[6], vertices[6], faces[6], fname);

    vertices[7] = vertices[6];
    faces[7] = faces[6];
    vertex_buffer_data[7] = vertex_buffer_data[6];
    index_buffer_data[7] = index_buffer_data[6];
    color_buffer_data[7] = color_buffer_data[6];
    ModelMatrix[7] = malloc(16*sizeof(float));
    TranslateOrigin[7] = malloc(16*sizeof(float));
    RotateX[7] = malloc(16*sizeof(float));
    RotateY[7] = malloc(16*sizeof(float));
    RotateZ[7] = malloc(16*sizeof(float));
    InitialTransform[7] = malloc(16*sizeof(float));
    UniformScale[7] = malloc(16*sizeof(float));

    vertices[8] = vertices[6];
    faces[8] = faces[6];
    vertex_buffer_data[8] = vertex_buffer_data[6];
    index_buffer_data[8] = index_buffer_data[6];
    color_buffer_data[8] = color_buffer_data[6];
    ModelMatrix[8] = malloc(16*sizeof(float));
    TranslateOrigin[8] = malloc(16*sizeof(float));
    RotateX[8] = malloc(16*sizeof(float));
    RotateY[8] = malloc(16*sizeof(float));
    RotateZ[8] = malloc(16*sizeof(float));
    InitialTransform[8] = malloc(16*sizeof(float));
    UniformScale[8] = malloc(16*sizeof(float));

    vertices[9] = vertices[6];
    faces[9] = faces[6];
    vertex_buffer_data[9] = vertex_buffer_data[6];
    index_buffer_data[9] = index_buffer_data[6];
    color_buffer_data[9] = color_buffer_data[6];
    ModelMatrix[9] = malloc(16*sizeof(float));
    TranslateOrigin[9] = malloc(16*sizeof(float));
    RotateX[9] = malloc(16*sizeof(float));
    RotateY[9] = malloc(16*sizeof(float));
    RotateZ[9] = malloc(16*sizeof(float));
    InitialTransform[9] = malloc(16*sizeof(float));
    UniformScale[9] = malloc(16*sizeof(float));

    /* Initialize GLUT; set double buffered window and RGBA color model */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 400);
    glutCreateWindow("MerryGoRound");

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
    glutMainLoop();

    free(VBO);
    free(IBO);
    free(CBO);
    free(vertices);
    free(faces);
    for(int i=0; i<objects; i++) {
      free(vertex_buffer_data[i]);
      free(index_buffer_data[i]);
      free(color_buffer_data[i]);
      free(ModelMatrix[i]);
      free(TranslateOrigin[i]);
      free(RotateX[i]);
      free(RotateY[i]);
      free(RotateZ[i]);
      free(InitialTransform[i]);
      free(UniformScale[i]);
    }
    free(vertex_buffer_data);
    free(index_buffer_data);
    free(color_buffer_data);
    free(ModelMatrix);
    free(TranslateOrigin);
    free(RotateX);
    free(RotateY);
    free(RotateZ);
    free(InitialTransform);
    free(UniformScale);
    /* ISO C requires main to return int */
    return 0;
}
