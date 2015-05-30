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
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif


/* Local includes */
#include "LoadShader.h"   /* Provides loading function for shader code */
#include "Mesh.h"
#include "Matrix.h"
#include "Vector.h"
#include "Bezier.h"
#include "OBJParser.h"


/*----------------------------------------------------------------*/

typedef struct {
  mesh* m;
  GLuint VBO;
  GLuint IBO;
  GLuint CBO;
  GLuint NBO;
  int vertices;
  int faces;
  GLfloat* vertex_buffer_data;
  GLushort* index_buffer_data;
  GLfloat* color_buffer_data;
  GLfloat* normal_buffer_data;
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
  vector* ctr;
  vector* eye;
  vector* up;
  vector* u;
  vector* v;
  vector* w;
} camera;

camera cam;

// variables for camera path
// =========================
int active = 0;
int direction = -1;
float t = 0.0;
vector* p1;
vector* p2;
vector* p3;
// =========================

// Light
GLfloat lightPos0[4];
GLfloat lightColor0[4];

typedef struct {
  int x;
  int y;
} position;

position mouse;

/* Define handle to a vertex array object */
GLuint VAO;

int count = 0;

/* Indices to vertex attributes; in this case positon and color */
enum DataID {vPosition = 0, vColor = 1, vNormal = 2};

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;
static const char* GeometryShaderString;

GLuint ShaderProgram;
GLuint ShaderProgram2;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */

/* Transformation matrices for initial position */
float TranslateDown[16];

void generateGeneralColorBuffer(GLfloat* result, int vertices, GLfloat r, GLfloat g, GLfloat b) {
  GLfloat temp[vertices*3];
  for(int i=0; i<vertices; i++) {
      temp[i*3] = r;
      temp[i*3+1] = g;
      temp[i*3+2] = b;
  }

  memcpy(result, temp, vertices*3*sizeof(GLfloat));
}

void generateCuboidVertexBuffer(GLfloat* result, GLfloat width, GLfloat length, GLfloat height) {
  GLfloat w = width/2.0;
  GLfloat h = height/2.0;
  GLfloat l = length/2.0;
  GLfloat temp[24] = {
    -w, -h,  l, // 0 --+
     w, -h,  l, // 1 +-+
    -w,  h,  l, // 2 -++
     w,  h,  l, // 3 +++
    -w, -h, -l, // 4 ---
     w, -h, -l, // 5 +--
    -w,  h, -l, // 6 -+-
     w,  h, -l  // 7 ++-
  };

  memcpy(result, temp, 24*sizeof(GLfloat));
}

void generateCuboidIndexBuffer(GLushort* result) {
  GLushort temp[48] = {
    0,1,3,
    0,3,2,
    1,5,7,
    1,7,3,
    5,4,6,
    5,6,7,
    4,0,2,
    4,2,6,
    4,5,1,
    4,1,0,
    2,3,7,
    2,7,6
  };

  memcpy(result, temp, 48*sizeof(GLushort));
}

void generateCuboid(object* obj, GLfloat width, GLfloat length, GLfloat height, GLfloat r, GLfloat g, GLfloat b) {
  generateCuboidVertexBuffer(obj->vertex_buffer_data, width, length, height);
  generateCuboidIndexBuffer(obj->index_buffer_data);
  generateGeneralColorBuffer(obj->color_buffer_data, obj->vertices, r, g, b);
  obj->m = createMesh(obj->vertex_buffer_data, obj->index_buffer_data, obj->color_buffer_data, obj->normal_buffer_data, obj->faces, obj->vertices);
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

void generateOctagonalPrismIndexBuffer(GLushort* result) {
  GLushort temp[96] = {
      0, 2 , 1 ,
      0, 3 , 2 ,
      0, 4 , 3 ,
      0, 5 , 4 ,
      0, 6 , 5 ,
      0, 7 , 6 ,
      0, 8 , 7 ,
      0, 1 , 8 ,
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
      10,  1,11,
      11,  2,12,
      12,  3,13,
      13,  4,14,
      14,  5,15,
      15,  6,16,
      16,  7,17,
      17,  8,10
  };

  memcpy(result, temp, 96*sizeof(GLushort));
}

void generateOctagonalPrism(object* obj, GLfloat radius, GLfloat height, GLfloat r, GLfloat g, GLfloat b) {
  generateOctagonalPrismVertexBuffer(obj->vertex_buffer_data, radius, height);
  generateOctagonalPrismIndexBuffer(obj->index_buffer_data);
  generateGeneralColorBuffer(obj->color_buffer_data, obj->vertices, r, g, b);
  obj->m = createMesh(obj->vertex_buffer_data, obj->index_buffer_data, obj->color_buffer_data, obj->normal_buffer_data, obj->faces, obj->vertices);
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

    for(int asdf = 0; asdf<2; asdf++) {
      if(asdf==0) glUseProgram(ShaderProgram);
      else glUseProgram(ShaderProgram2);
    for(int i=0; i<count; i++) {
      glEnableVertexAttribArray(vPosition);
      glBindBuffer(GL_ARRAY_BUFFER, objects[i].VBO);
      glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

      glEnableVertexAttribArray(vColor);
      glBindBuffer(GL_ARRAY_BUFFER, objects[i].CBO);
      glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);

      glEnableVertexAttribArray(vNormal);
      glBindBuffer(GL_ARRAY_BUFFER, objects[i].NBO);
      glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

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

      GLint LightUniform = glGetUniformLocation(ShaderProgram, "LightPosition");
      if (LightUniform == -1) {
        fprintf(stderr, "Could not bind uniform LightPosition");
        exit(-1);
      }
      /*lightPos0[0] = *cam.eye->x;
      lightPos0[1] = *cam.eye->y;
      lightPos0[2] = *cam.eye->z;
      lightPos0[3] = 0.0f;*/
      glUniform4fv(LightUniform, 1, lightPos0);
      /* Set state to only draw wireframe (no lighting used, yet) */
      //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      /* Issue draw command, using indexed triangle list */
      glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

      /* Disable attributes */
      glDisableVertexAttribArray(vPosition);
      glDisableVertexAttribArray(vColor);
      glDisableVertexAttribArray(vNormal);
    }
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
  SetTranslation(-(*cam.eye->x), -(*cam.eye->y), -(*cam.eye->z), ViewMatrix);
  float temp[16] = {
    *cam.u->x, *cam.u->y, *cam.u->z, 0.0,
    *cam.v->x, *cam.v->y, *cam.v->z, 0.0,
    *cam.w->x, *cam.w->y, *cam.w->z, 0.0,
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
         *cam.eye->x=(*p1->x); *cam.eye->y=(*p1->y); *cam.eye->z=(*p1->z);
         break;
       }
       case 'q':
       case 'Q': {
         for(int i=0; i<count; i++) {
            free(objects[i].vertex_buffer_data);
            free(objects[i].index_buffer_data);
            free(objects[i].color_buffer_data);
            free(objects[i].normal_buffer_data);
            if(count<11) destroyMesh(objects[i].m);
          }
          free(objects);
          destroyVector(p1);
          destroyVector(p2);
          destroyVector(p3);
          destroyVector(cam.ctr);
          destroyVector(cam.eye);
          destroyVector(cam.up);
          destroyVector(cam.u);
          destroyVector(cam.v);
          destroyVector(cam.w);
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
    for(int i=0; i<6; i++) {
      MultiplyMatrix(RotationMatrixAnim, objects[i].InitialTransform, objects[i].ModelMatrix);
    }

    for(int i=11; i<count; i++) {
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
      *cam.ctr->x=0.0f; *cam.ctr->y=0.0f; *cam.ctr->z=0.0f;
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

    for(int i=0; i<count; i++) {
      glGenBuffers(1, &objects[i].CBO);
      glBindBuffer(GL_ARRAY_BUFFER, objects[i].CBO);
      glBufferData(GL_ARRAY_BUFFER, objects[i].vertices*3*sizeof(GLfloat), objects[i].color_buffer_data, GL_STATIC_DRAW);
    }

    for(int i=0; i<count; i++) {
      glGenBuffers(1, &objects[i].NBO);
      glBindBuffer(GL_ARRAY_BUFFER, objects[i].NBO);
      glBufferData(GL_ARRAY_BUFFER, objects[i].vertices*3*sizeof(GLfloat), objects[i].normal_buffer_data, GL_STATIC_DRAW);
    }
}


/******************************************************************
*
* AddShader
*
* This function creates and adds individual shaders
*
*******************************************************************/

void AddShader(GLuint sProgram, const char* ShaderCode, GLenum ShaderType)
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
    glAttachShader(sProgram, ShaderObj);
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

void CreateShaderProgram2()
{
    /* Allocate shader object */
    ShaderProgram2 = glCreateProgram();

    if (ShaderProgram == 0)
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    /* Load shader code from file */
    VertexShaderString = LoadShader("shaders/vertexshader2.vs");
    FragmentShaderString = LoadShader("shaders/fragmentshader2.fs");
    GeometryShaderString = LoadShader("shaders/geometryshader.gs");

    /* Separately add vertex and fragment shader to program */
    AddShader(ShaderProgram2, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram2, FragmentShaderString, GL_FRAGMENT_SHADER);
    AddShader(ShaderProgram2, GeometryShaderString, GL_GEOMETRY_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024];

    /* Link shader code into executable shader program */
    glLinkProgram(ShaderProgram2);

    /* Check results of linking step */
    glGetProgramiv(ShaderProgram2, GL_LINK_STATUS, &Success);

    if (Success == 0)
    {
        glGetProgramInfoLog(ShaderProgram2, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Check if shader program can be executed */
    glValidateProgram(ShaderProgram2);
    glGetProgramiv(ShaderProgram2, GL_VALIDATE_STATUS, &Success);

    if (!Success)
    {
        glGetProgramInfoLog(ShaderProgram2, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Put linked shader program into drawing pipeline */
    glUseProgram(ShaderProgram2);
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
    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColor);

    lightColor0[0] = 0.5f; lightColor0[1] = 0.5f; lightColor0[2] = 0.5f; lightColor0[3] = 1.0f;
    lightPos0[0] = 0.0f; lightPos0[1] = 100.0f; lightPos0[2] = 0.0f; lightPos0[3] = 0.0f;

    //GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f};
    //GLfloat lightPos1[] = {0.0f, 50.0f, 0.0f, 0.0f};
    //glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    //glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

    /* Setup vertex, color, and index buffer objects */
    SetupDataBuffers();

    /* Setup shaders and shader program */
    CreateShaderProgram();
    CreateShaderProgram2();

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
    p1 = createVector(40.0f, 40.0f, 40.0f); // camera path
    p2 = createVector(40.0f, 40.0f,  0.0f); // camera path
    p3 = createVector(40.0f,  0.0f,  0.0f); // camera path
    cam.ctr = createVector(0.0f, 0.0f, 0.0f);
    cam.eye = createVector(0.0f, 0.0f, 40.0f);
    cam.up = createVector(0.0f, 1.0f, 0.0f);
    cam.u = createVector(0.0f, 0.0f, 0.0f);
    cam.v = createVector(0.0f, 0.0f, 0.0f);
    cam.w = createVector(0.0f, 0.0f, 0.0f);
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
    SetTranslation(0, -12.51, 0, objects[6].ModelMatrix);

    // translate second cuboid (wall)
    SetTranslation(0.0, 1.25, 48.875, objects[7].ModelMatrix);

    // translate third cuboid (wall)
    SetTranslation(0.0, 1.25, -48.875, objects[8].ModelMatrix);

    // translate fourth cuboid (wall)
    SetTranslation(48.875, 1.25, 0.0, objects[9].ModelMatrix);

    // translate fifth cuboid (wall)
    SetTranslation(-48.875, 1.25, 0.0, objects[10].ModelMatrix);

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
      objects[i].normal_buffer_data = calloc(objects[i].vertices*3, sizeof(GLfloat));
    }
    generateOctagonalPrism(&objects[0],20.0,2.5,0.5f,0.5f,0.5f);
    generateOctagonalPrism(&objects[1],20.0,2.5,0.5f,0.5f,0.5f);
    generateOctagonalPrism(&objects[2],1.0,20,1.0f,0.0f,0.0f);
    generateOctagonalPrism(&objects[3],1.0,20,1.0f,0.0f,0.0f);
    generateOctagonalPrism(&objects[4],1.0,20,1.0f,0.0f,0.0f);
    generateOctagonalPrism(&objects[5],1.0,20,1.0f,0.0f,0.0f);

    for(int i=6; i<11; i++) {
      objects[i].vertices = 8;
      objects[i].faces = 12;
      objects[i].vertex_buffer_data = malloc(objects[i].vertices*3*sizeof(GLfloat));
      objects[i].index_buffer_data = malloc(objects[i].faces*3*sizeof(GLushort));
      objects[i].color_buffer_data = malloc(objects[i].vertices*3*sizeof(GLfloat));
      objects[i].normal_buffer_data = calloc(objects[i].vertices*3, sizeof(GLfloat));
    }
    generateCuboid(&objects[6],100.0,100.0,2.5,0.0f,1.0f,0.0f);
    generateCuboid(&objects[7],100.0,2.5,25.0,0.0f,0.0f,1.0f);
    generateCuboid(&objects[8],100.0,2.5,25.0,0.0f,0.0f,1.0f);
    generateCuboid(&objects[9],2.5,100.0,25.0,0.0f,0.0f,1.0f);
    generateCuboid(&objects[10],2.5,100.0,25.0,0.0f,0.0f,1.0f);

    char* fname="models/teddy.obj";
    obj_scene_data data;
    parse_obj_scene(&data, fname);

    objects[11].vertices = data.vertex_count;
    objects[11].faces = data.face_count;
    //printf("%d %d\n",objects[11].vertices,objects[11].faces);
    objects[11].vertex_buffer_data = malloc(objects[11].vertices*3*sizeof(GLfloat));
    objects[11].index_buffer_data = malloc(objects[11].faces*3*sizeof(GLushort));
    objects[11].color_buffer_data = malloc(objects[11].vertices*3*sizeof(GLfloat));
    objects[11].normal_buffer_data = calloc(objects[11].vertices*3, sizeof(GLfloat));

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
    generateGeneralColorBuffer(objects[11].color_buffer_data, objects[11].vertices, 0.5f, 0.35f, 0.05f);
    //objects[11].m = createMesh(objects[11].vertex_buffer_data, objects[11].index_buffer_data, objects[11].color_buffer_data, objects[11].normal_buffer_data, objects[11].vertices, objects[11].faces);

    for(int i=12; i<15; i++) {
      objects[i].vertices = objects[11].vertices;
      objects[i].faces = objects[11].faces;
      objects[i].vertex_buffer_data = malloc(objects[i].vertices*3*sizeof(GLfloat));
      objects[i].index_buffer_data = malloc(objects[i].faces*3*sizeof(GLushort));
      objects[i].color_buffer_data = malloc(objects[i].vertices*3*sizeof(GLfloat));
      objects[i].normal_buffer_data = calloc(objects[i].vertices*3, sizeof(GLfloat));
      //objects[i].m = createMesh(objects[i].vertex_buffer_data, objects[i].index_buffer_data, objects[i].color_buffer_data, objects[i].normal_buffer_data, objects[11].vertices, objects[11].faces);
      memcpy(objects[i].vertex_buffer_data, objects[11].vertex_buffer_data, objects[11].vertices*3*sizeof(GLfloat));
      memcpy(objects[i].index_buffer_data, objects[11].index_buffer_data, objects[11].faces*3*sizeof(GLushort));
      memcpy(objects[i].color_buffer_data, objects[11].color_buffer_data, objects[11].vertices*3*sizeof(GLfloat));
      memcpy(objects[i].normal_buffer_data, objects[11].normal_buffer_data, objects[11].vertices*3*sizeof(GLfloat));
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
