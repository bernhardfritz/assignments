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
#include "Vector.h"
#include "Bezier.h"
#include "OBJParser.h"


/*----------------------------------------------------------------*/

typedef struct {
  GLuint VBO;
  GLuint IBO;
  GLuint CBO;
  int vertices;
  int faces;
  GLfloat* vertex_buffer_data;
  GLushort* index_buffer_data;
  GLfloat* color_buffer_data;
  float ModelMatrix[16];
  float TranslateOrigin[16];
  float RotateX[16];
  float RotateY[16];
  float RotateZ[16];
  float InitialTransform[16];
  float UniformScale[16];
} object;

object* objects;

typedef struct {
  float ctr[3];
  float eye[3];
  float up[3];
  float u[3];
  float v[3];
  float w[3];
} camera;

camera cam;

// variables for camera path
// =========================
active = 0;
direction = -1;
float p1[3] = { 50.0, 50.0, 50.0};
float p2[3] = { 50.0, 50.0,  0.0};
float p3[3] = { 50.0,  0.0,  0.0};
float t = 0.0;
// =========================

typedef struct {
  int x;
  int y;
} position;

position mouse;

/* Define handle to a vertex array object */
GLuint VAO;

int count = 0;

/* Indices to vertex attributes; in this case positon and color */
enum DataID {vPosition = 0, vColor = 1};

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */

/* Transformation matrices for initial position */
float TranslateDown[16];

void generateCuboidVertexBuffer(GLfloat* result, GLfloat width, GLfloat length, GLfloat height) {
  GLfloat temp[24] = {
    width/2.0,          height/2.0,         length/2.0,
    width/2.0,          height/2.0,         -length/2.0,
    width/2.0,          -height/2.0,        length/2.0,
    width/2.0,          -height/2.0,        -length/2.0,
    -width/2.0,         height/2.0,         length/2.0,
    -width/2.0,         height/2.0,         -length/2.0,
    -width/2.0,         -height/2.0,        length/2.0,
    -width/2.0,         -height/2.0,        -length/2.0
  };

  memcpy(result, temp, 24*sizeof(GLfloat));
}

void generateCuboidColorBuffer(GLfloat* result) {
  GLfloat temp[24] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.1,
    0.0, 1.0, 0.0,
    0.0, 1.0, 1.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 1.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 1.0
  };

  memcpy(result, temp, 24*sizeof(GLfloat));
}

void generateCuboidIndexBuffer(GLushort* result) {
  GLushort temp[48] = {
    0, 1, 2,
    1, 2, 3,
    1, 3, 5,
    3, 5, 7,
    4, 5, 7,
    4, 6, 7,
    0, 3, 6,
    0, 5, 6,
    0, 1, 4,
    1, 4, 5,
    2, 3, 6,
    3, 6, 7
  };

  memcpy(result, temp, 48*sizeof(GLushort));
}

void generateCuboid(GLfloat* result_vertex_buffer_data, GLushort* result_index_buffer_data, GLfloat* result_color_buffer_data, GLfloat width, GLfloat length, GLfloat height) {
  generateCuboidVertexBuffer(result_vertex_buffer_data, width, length, height);
  generateCuboidIndexBuffer(result_index_buffer_data);
  generateCuboidColorBuffer(result_color_buffer_data);
}

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

    for(int i=0; i<count; i++) {
      glEnableVertexAttribArray(vPosition);
      glBindBuffer(GL_ARRAY_BUFFER, objects[i].VBO);
      glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[i].IBO);
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
      glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, objects[i].ModelMatrix);

      /* Set state to only draw wireframe (no lighting used, yet) */
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      /* Issue draw command, using indexed triangle list */
      glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

      /* Disable attributes */
      glDisableVertexAttribArray(vPosition);
    }

    /* Swap between front and back buffer */
    glutSwapBuffers();
}

void updateCamera() {
  SubtractVector(cam.eye, cam.ctr, cam.w);
  SetUnitVector(cam.w);
  MultiplyVector(cam.up, cam.w, cam.u);
  SetUnitVector(cam.u);
  MultiplyVector(cam.w, cam.u, cam.v);

  SetIdentityMatrix(ViewMatrix);
  /* Set viewing transform */
  SetTranslation(-cam.eye[0], -cam.eye[1], -cam.eye[2], ViewMatrix);
  float temp[16] = {
    cam.u[0], cam.u[1], cam.u[2], 0.0,
    cam.v[0], cam.v[1], cam.v[2], 0.0,
    cam.w[0], cam.w[1], cam.w[2], 0.0,
         0.0,      0.0,      0.0, 1.0
  };
  MultiplyMatrix(temp,ViewMatrix,ViewMatrix);
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

void Mouse(int button, int state, int x, int y) {
    if(state == GLUT_DOWN) {
       switch(button) {
	        case GLUT_LEFT_BUTTON: break;
          case GLUT_MIDDLE_BUTTON: break;
	        case GLUT_RIGHT_BUTTON: break;
	     }
    }
}

int once = 1;
void Motion(int x, int y) {
  if(once) {
    once = 0;
    goto done;
  }
  if(x == mouse.x && y == mouse.y) return;
  //printf("x:%d y:%d\n",x,y);
  if(x < mouse.x) SubtractVector(cam.ctr, cam.u, cam.ctr);
  else if(x > mouse.x) AddVector(cam.ctr, cam.u, cam.ctr);
  if(y < mouse.y) AddVector(cam.ctr, cam.up, cam.ctr);
  else if(y > mouse.y) SubtractVector(cam.ctr, cam.up, cam.ctr);
  updateCamera();
  glutPostRedisplay();
  done:
  mouse.x = x;
  mouse.y = y;
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

void Keyboard(unsigned char key, int x, int y) {
    switch(key) {
	     case 'w': {
         SubtractVector(cam.eye, cam.w, cam.eye);
         SubtractVector(cam.ctr, cam.w, cam.ctr);
         break;
       }
	     case 'a': {
         SubtractVector(cam.eye, cam.u, cam.eye);
         SubtractVector(cam.ctr, cam.u, cam.ctr);
         break;
       }
       case 's': {
         AddVector(cam.eye, cam.w, cam.eye);
         AddVector(cam.ctr, cam.w, cam.ctr);
         break;
       }
	     case 'd': {
         AddVector(cam.eye, cam.u, cam.eye);
         AddVector(cam.ctr, cam.u, cam.ctr);
         break;
       }
       case 'r': {
         AddVector(cam.eye, cam.up, cam.eye);
         AddVector(cam.ctr, cam.up, cam.ctr);
         break;
       }
       case 'f': {
         SubtractVector(cam.eye, cam.up, cam.eye);
         SubtractVector(cam.ctr, cam.up, cam.ctr);
         break;
       }
       case 'c': {
         active = 1;
         direction *= -1;
         cam.eye[0]=p1[0]; cam.eye[1]=p1[1]; cam.eye[2]=p1[2];
         break;
       }
       case 'q':
       case 'Q': {
         for(int i=0; i<count; i++) {
            free(objects[i].vertex_buffer_data);
            free(objects[i].index_buffer_data);
            free(objects[i].color_buffer_data);
          }
          free(objects);
          exit(0);
          break;
       }
    }

    updateCamera();

    glutPostRedisplay();
}

void Special(int key, int x, int y) {
  switch(key) {
    case GLUT_KEY_UP: AddVector(cam.ctr, cam.up, cam.ctr); break;
    case GLUT_KEY_LEFT: SubtractVector(cam.ctr, cam.u, cam.ctr); break;
    case GLUT_KEY_DOWN: SubtractVector(cam.ctr, cam.up, cam.ctr); break;
    case GLUT_KEY_RIGHT: AddVector(cam.ctr, cam.u, cam.ctr); break;
  }

  updateCamera();

  glutPostRedisplay();
}


/******************************************************************
*
* OnIdle
*
*******************************************************************/
void OnIdle()
{
    float angle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (180.0/M_PI);
    float RotationMatrixAnim[16];

    /* Time dependent rotation */
    SetRotationY(angle, RotationMatrixAnim);

    /* Apply model rotation */
    for(int i=0; i<count; i++) {
      MultiplyMatrix(RotationMatrixAnim, objects[i].InitialTransform, objects[i].ModelMatrix);
    }

    /* Time dependent translation */
    for(int i=11; i<count; i++) {
      float distance = sinf((angle+(i-6)*90)/16)*4;
      float TranslationMatrixAnim[16];
      SetTranslation(0, distance, 0, TranslationMatrixAnim);
      MultiplyMatrix(TranslationMatrixAnim, objects[i].ModelMatrix, objects[i].ModelMatrix);
    }

    if(active) {
      quadratic_bezier(cam.eye, p1, p2, p3, t);
      updateCamera();
      cam.ctr[0]=0.0; cam.ctr[1]=0.0; cam.ctr[2]=0.0;
      if((direction==1 && t>=1.0) || (direction==-1 && t<=0.0)) {
        active=0;
      } else {
        t+=direction*0.01;
      }
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
    for(int i=0; i<count; i++) {
      glGenBuffers(1, &objects[i].VBO);
      glBindBuffer(GL_ARRAY_BUFFER, objects[i].VBO);
      glBufferData(GL_ARRAY_BUFFER, objects[i].vertices*3*sizeof(GLfloat), objects[i].vertex_buffer_data, GL_STATIC_DRAW);
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    for(int i=0; i<count; i++) {
      glGenBuffers(1, &objects[i].IBO);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[i].IBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, objects[i].faces*3*sizeof(GLushort), objects[i].index_buffer_data, GL_STATIC_DRAW);
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
* vertex buffer objects, and to setup the vertex and fragment shader
*
*******************************************************************/

void Initialize()
{
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
    for(int i=0; i<count; i++) {
      SetIdentityMatrix(objects[i].ModelMatrix);
      SetIdentityMatrix(objects[i].InitialTransform);
    }

    /* Set projection transform */
    float fovy = 45.0;
    float aspect = 1.0;
    float nearPlane = 1.0;
    float farPlane = 200.0;
    SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);

    /* Initialize camera */
    cam.ctr[0] = 0.0; cam.ctr[1] = 0.0; cam.ctr[2] = 0.0;
    cam.eye[0] = 0.0; cam.eye[1] = 0.0; cam.eye[2] = 50.0;
    cam.up[0] = 0.0; cam.up[1] = 1.0; cam.up[2] = 0.0;
    updateCamera();

    //printf("width:%d height:%d\n",GLUT_WINDOW_WIDTH,GLUT_WINDOW_HEIGHT);

    // translate bottom plane
    SetTranslation(0, 0, 10, objects[0].TranslateOrigin);
    SetRotationX(90, objects[0].RotateX);
    MultiplyMatrix(objects[0].RotateX, objects[0].TranslateOrigin, objects[0].InitialTransform);

    // translate top plane
    SetTranslation(0, 0, -10, objects[1].TranslateOrigin);
    SetRotationX(90, objects[1].RotateX);
    MultiplyMatrix(objects[1].RotateX, objects[1].TranslateOrigin, objects[1].InitialTransform);

    // translate first column
    SetTranslation(12.5, 0, 0, objects[2].TranslateOrigin);
    SetRotationX(90, objects[2].RotateX);
    MultiplyMatrix(objects[2].RotateX, objects[2].TranslateOrigin, objects[2].InitialTransform);

    // translate second column
    SetTranslation(-12.5, 0, 0, objects[3].TranslateOrigin);
    SetRotationX(90, objects[3].RotateX);
    MultiplyMatrix(objects[3].RotateX, objects[3].TranslateOrigin, objects[3].InitialTransform);

    // translate third column
    SetTranslation(0, 12.5, 0, objects[4].TranslateOrigin);
    SetRotationX(90, objects[4].RotateX);
    MultiplyMatrix(objects[4].RotateX, objects[4].TranslateOrigin, objects[4].InitialTransform);

    // translate fourth column
    SetTranslation(0, -12.5, 0, objects[5].TranslateOrigin);
    SetRotationX(90, objects[5].RotateX);
    MultiplyMatrix(objects[5].RotateX, objects[5].TranslateOrigin, objects[5].InitialTransform);

    // transate first cuboid (floor)
    SetTranslation(0, -12.5, 0, objects[6].InitialTransform);

    // translate second cuboid (wall)
    SetTranslation(0.0, 1.25, 48.875, objects[7].InitialTransform);

    // translate third cuboid (wall)
    SetTranslation(0.0, 1.25, -48.875, objects[8].InitialTransform);

    // translate fourth cuboid (wall)
    SetTranslation(48.875, 1.25, 0.0, objects[9].InitialTransform);

    // translate fifth cuboid (wall)
    SetTranslation(-48.875, 1.25, 0.0, objects[10].InitialTransform);

    // rotate, translate and scale first teddy
    SetTranslation(-50, 0, 0, objects[11].TranslateOrigin);
    SetRotationY(0, objects[11].RotateY);
    SetUniformScale(0.25, objects[11].UniformScale);
    MultiplyMatrix(objects[11].RotateY, objects[11].TranslateOrigin, objects[11].InitialTransform);
    MultiplyMatrix(objects[11].UniformScale, objects[11].InitialTransform, objects[11].InitialTransform);

    // rotate, translate and scale second teddy
    SetTranslation(-50, 0, 0, objects[12].TranslateOrigin);
    SetRotationY(90, objects[12].RotateY);
    SetUniformScale(0.25, objects[12].UniformScale);
    MultiplyMatrix(objects[12].RotateY, objects[12].TranslateOrigin, objects[12].InitialTransform);
    MultiplyMatrix(objects[12].UniformScale, objects[12].InitialTransform, objects[12].InitialTransform);

    // rotate, translate and scale third teddy
    SetTranslation(-50, 0, 0, objects[13].TranslateOrigin);
    SetRotationY(180, objects[13].RotateY);
    SetUniformScale(0.25, objects[13].UniformScale);
    MultiplyMatrix(objects[13].RotateY, objects[13].TranslateOrigin, objects[13].InitialTransform);
    MultiplyMatrix(objects[13].UniformScale, objects[13].InitialTransform, objects[13].InitialTransform);

    // rotate, translate and scale fourth teddy
    SetTranslation(-50, 0, 0, objects[14].TranslateOrigin);
    SetRotationY(270, objects[14].RotateY);
    SetUniformScale(0.25, objects[14].UniformScale);
    MultiplyMatrix(objects[14].RotateY, objects[14].TranslateOrigin, objects[14].InitialTransform);
    MultiplyMatrix(objects[14].UniformScale, objects[14].InitialTransform, objects[14].InitialTransform);
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
    count = 15;
    objects = malloc(count*sizeof(object));

    for(int i=0; i<6; i++) {
      objects[i].vertices=18;
      objects[i].faces=32;
      objects[i].vertex_buffer_data = malloc(objects[i].vertices*3*sizeof(GLfloat));
      objects[i].index_buffer_data = malloc(objects[i].faces*3*sizeof(GLfloat));
      objects[i].color_buffer_data = malloc(objects[i].vertices*3*sizeof(GLfloat));
    }
    generateOctagonalPrism(objects[0].vertex_buffer_data,objects[0].index_buffer_data,objects[0].color_buffer_data,20.0,2.5);
    generateOctagonalPrism(objects[1].vertex_buffer_data,objects[1].index_buffer_data,objects[1].color_buffer_data,20.0,2.5);
    generateOctagonalPrism(objects[2].vertex_buffer_data,objects[2].index_buffer_data,objects[2].color_buffer_data,1.0,20);
    generateOctagonalPrism(objects[3].vertex_buffer_data,objects[3].index_buffer_data,objects[3].color_buffer_data,1.0,20);
    generateOctagonalPrism(objects[4].vertex_buffer_data,objects[4].index_buffer_data,objects[4].color_buffer_data,1.0,20);
    generateOctagonalPrism(objects[5].vertex_buffer_data,objects[5].index_buffer_data,objects[5].color_buffer_data,1.0,20);

    for(int i=6; i<11; i++) {
      objects[i].vertices = 8;
      objects[i].faces = 12;
      objects[i].vertex_buffer_data = malloc(objects[i].vertices*3*sizeof(GLfloat));
      objects[i].index_buffer_data = malloc(objects[i].faces*3*sizeof(GLushort));
      objects[i].color_buffer_data = malloc(objects[i].vertices*3*sizeof(GLfloat));
    }
    generateCuboid(objects[6].vertex_buffer_data,objects[6].index_buffer_data,objects[6].color_buffer_data,100.0,100.0,2.5);
    generateCuboid(objects[7].vertex_buffer_data,objects[7].index_buffer_data,objects[7].color_buffer_data,100.0,2.5,25.0);
    generateCuboid(objects[8].vertex_buffer_data,objects[8].index_buffer_data,objects[8].color_buffer_data,100.0,2.5,25.0);
    generateCuboid(objects[9].vertex_buffer_data,objects[9].index_buffer_data,objects[9].color_buffer_data,2.5,100.0,25.0);
    generateCuboid(objects[10].vertex_buffer_data,objects[10].index_buffer_data,objects[10].color_buffer_data,2.5,100.0,25.0);

    char* fname="models/teddy.obj";
    obj_scene_data data;
    parse_obj_scene(&data, fname);

    objects[11].vertices = data.vertex_count;
    objects[11].faces = data.face_count;
    //printf("%d %d\n",objects[6].vertices,objects[6].faces);
    objects[11].vertex_buffer_data = malloc(objects[11].vertices*3*sizeof(GLfloat));
    objects[11].index_buffer_data = malloc(objects[11].faces*3*sizeof(GLushort));
    objects[11].color_buffer_data = malloc(objects[11].vertices*3*sizeof(GLfloat));
    // Vertices
    for(int i=0; i<objects[11].vertices; i++) {
        objects[11].vertex_buffer_data[i*3] = (GLfloat)(*data.vertex_list[i]).e[0];
	      objects[11].vertex_buffer_data[i*3+1] = (GLfloat)(*data.vertex_list[i]).e[1];
	      objects[11].vertex_buffer_data[i*3+2] = (GLfloat)(*data.vertex_list[i]).e[2];
    }
    // Indices
    for(int i=0; i<objects[11].faces; i++) {
	      objects[11].index_buffer_data[i*3] = (GLushort)(*data.face_list[i]).vertex_index[0];
	      objects[11].index_buffer_data[i*3+1] = (GLushort)(*data.face_list[i]).vertex_index[1];
	      objects[11].index_buffer_data[i*3+2] = (GLushort)(*data.face_list[i]).vertex_index[2];
    }

    for(int i=12; i<15; i++) {
      objects[i].vertices = objects[11].vertices;
      objects[i].faces = objects[11].faces;
      objects[i].vertex_buffer_data = malloc(objects[i].vertices*3*sizeof(GLfloat));
      objects[i].index_buffer_data = malloc(objects[i].faces*3*sizeof(GLushort));
      objects[i].color_buffer_data = malloc(objects[i].vertices*3*sizeof(GLfloat));
      memcpy(objects[i].vertex_buffer_data, objects[11].vertex_buffer_data, objects[11].vertices*3*sizeof(GLfloat));
      memcpy(objects[i].index_buffer_data, objects[11].index_buffer_data, objects[11].faces*3*sizeof(GLushort));
      memcpy(objects[i].color_buffer_data, objects[11].color_buffer_data, objects[11].vertices*3*sizeof(GLfloat));
    }

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
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(Special);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutMainLoop();

    /* ISO C requires main to return int */
    return 0;
}
