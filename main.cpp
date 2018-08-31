/*****************************************************************************
 * Michael Usner
 * CS433 - Asignment 3
 * November 6, 2003
 *
 * main.cpp
 *****************************************************************************/
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "cube.h"

#define SCREEN 4.8
#define WINDOW_X        640    /* Window width */
#define WINDOW_Y        640    /* Window height */
#define BUFSIZE 512     //pick buffer size
#define MAX_SCRAMBLE 20

int scrambleIter = MAX_SCRAMBLE;    // scramble iterations

RubiksCube cube;
extern bool cubeRotating;       // cube is animating...
extern int DEG;                 // used for rotating and scrambling

// forward declarations
bool scramble();

// light properties
GLfloat light_position[] = { 0.0f, 0.3f, 1.0f, 0.0f };
GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };

float damp  = 0.6f;      // mouse movement dampening factor
float zoomdamp = 0.01f;

// initial rotation positions
float scenex= 30.0,//-60.0,
      sceney= 35.0,
      scenez= 0.0;//-25.0;

float scenezoom = 1.7f;
extern float selAnim;

// the scene rotation status
bool  rotating  = false;    // rotating model
bool zooming    = false;    // zooming on model
bool selected   = false;    // side is selected
bool rotateFace = false;    // rotating a cube face
bool scrambling = false;    // scrambling status

// viewport variables
int vpw = WINDOW_X, vph = WINDOW_Y;

// the name of the last hit
static unsigned int lastname=0;

// the mouse x and y locations
GLfloat mousexSave,
        mouseySave;

// the render mode
GLenum mode;
bool rotatingCW = false;


// initialize OpenGL
void init(void)
{
    glClearColor (1.0f,1.0f,1.0f,0.0f);
    glShadeModel (GL_SMOOTH);
    
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);
    glLightfv (GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv (GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
       
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glEnable(GL_COLOR_MATERIAL);
    srand(time(NULL));      // seed the RNG
}

// draw the scene
void draw(GLenum mode)
{
    glPushMatrix();
    /* rotate scene relative to mouse movement */
    glRotatef(scenex, 1.0, 0.0, 0.0);
    glRotatef(sceney, 0.0, 1.0, 0.0);
    glRotatef(scenez, 0.0, 0.0, 1.0);
    glScalef(scenezoom, scenezoom, scenezoom);

    
    cube.draw();
    glPopMatrix();
}

// the display callback
// draw the scores, message, and scene
void display ()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw(GL_RENDER);
    glutSwapBuffers();
}

void timer(int i)
{
    // update the stimulus variable in cube for selections
    selAnim += 0.1f;
    if (selAnim>=3.14f)
        selAnim=0.f;

    if (rotatingCW && cubeRotating)
        cube.rotate(true);
    else if (!rotatingCW && cubeRotating)
        cube.rotate(false);

    if (scrambling)
    {
        scramble();
        scenex += 5;
        sceney += 5;
    }
    glutPostRedisplay();
    glutTimerFunc(30,timer,0);
}

// scramble the cube
bool scramble()
{
    int face;
    int dir;

    if (scrambleIter==0) 
    {
        scrambling = false;
        return false;
    }

    if (!cubeRotating)
    {       
        // pick a face
        face = rand()%7;
        if (face==0) face=1;
        if (face>6) face=6;

        dir = rand()%2;
        cube.setSelected(face);
        if (dir==0)
        {
            cube.rotate(false);
            rotatingCW = false;
        }
        else
        {
            cube.rotate(true);
            rotatingCW = true;
        }
        scrambleIter--;
        return true;
    }
    return false;
}


// the reshape callback
void reshape(int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
   
    vpw=w;
    vph=h;
    if (w>=h)
        gluPerspective(60.0, w/h, 0.1, 30);
    else
        gluPerspective(60.0, h/w, 0.1, 30);
   
    gluLookAt(
        0.0, 0.0, 9.0,      /* eye is at (0,0,5) */
        0.0, 0.0, 0.0,      /* center is at (0,0,0) */
        0.0, 1.0, 0.);      /* up is in positive Y direction */
  glMatrixMode(GL_MODELVIEW);
}

// the keyboard callback
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case (27):
        exit(0);
        break;
    case ('s'):
        scrambleIter = MAX_SCRAMBLE;
        scrambling = true;
        //scramble();
        break;
   default:
        break;
    }
}

/* this gets called if the mouse button is pressed or released */
void mouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mousexSave = x;
            mouseySave = y;
            if (selected)
                rotateFace = true;
            else
                rotating = true;
        } 
        else 
        {
            rotating = false;
            rotateFace = false;
        }
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mousexSave = x;
            mouseySave = y;
            zooming=true;
        }
        else
            zooming=false;
    }
}

// process the hits from the selection buffer and 
// highlight the square if the mouse is hovering
void processHits(GLuint hits, GLuint buffer[])
{
    unsigned int i;
   GLuint names, *ptr, minZ,*ptrNames, numberOfNames;

   cube.clearSelected();
   selected=false;
   if (hits==0) return;
   ptr = (GLuint *) buffer;
   minZ = 0xffffffff;
   for (i = 0; i < hits; i++) {	
      names = *ptr;
	  ptr++;
	  if (*ptr < minZ) {
		  numberOfNames = names;
		  minZ = *ptr;
		  ptrNames = ptr+2;
	  }
	  
	  ptr += names+2;
	}
   if (numberOfNames>0)
        if (*ptrNames != 0)
        {
            cube.setSelected(*ptrNames);
            selected = true;    // lets mouse motion know we're moving a face
        }
}


/* this gets called if the mouse is moved while a button is pressed. */
void mouseMove(int x, int y)
{
    GLuint selectBuf[BUFSIZE];
    GLint hits;
    GLint viewport[4];

    if (rotating)   // rotate the scene 
    {
        if (x > mousexSave) 
            sceney += (x-mousexSave)*damp;
        else if (x < mousexSave) 
            sceney -=(mousexSave-x)*damp;
        if (y > mouseySave) 
            scenex += (y-mouseySave)*damp;
        else if (y < mouseySave) 
            scenex -= +(mouseySave-y)*damp;
    
        
        glutPostRedisplay();
        mousexSave = x;
        mouseySave = y;
        return;
    }
    else if(zooming)    // zoom the scene
    {
        if (y > mouseySave) 
            scenezoom -= (y-mouseySave)*zoomdamp;
        else if (y < mouseySave) 
            scenezoom += +(mouseySave-y)*zoomdamp;
    }
    else if (selected && rotateFace)    // rotate a face of the cube
    {
        if (mouseySave < y) 
        {
            cube.rotate(false);
            rotatingCW = false;
        }
        else if (mouseySave > y) 
        {
            cube.rotate(true);
            rotatingCW = true;
        }
        rotateFace = false;
    }
    mousexSave = x;
    mouseySave = y;

    if (cubeRotating) return;

    // use picking to determine the square that the mouse is over
    glMatrixMode (GL_PROJECTION);
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(BUFSIZE, selectBuf);
    (void) glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluPickMatrix((GLdouble)x, (GLdouble) (viewport[3] - y), 5.0, 5.0, viewport);

        if (vpw>=vph)
         gluPerspective(60.0, vpw/vph, 0.1, 30);
     else
         gluPerspective(60.0, vph/vpw, 0.1, 30);
         
         gluLookAt(
            0.0, 0.0, 9.0,      /* eye is at (0,0,9) */
            0.0, 0.0, 0.0,      /* center is at (0,0,0) */
            0.0, 1.0, 0.);      /* up is in positive Y direction */
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();
        glLoadName(1);
        draw(GL_SELECT);
        glPopMatrix();
       glMatrixMode(GL_PROJECTION);   // Select The Projection Matrix
     glPopMatrix();
    hits = glRenderMode(GL_RENDER); 
    processHits(hits, selectBuf);
    glMatrixMode(GL_MODELVIEW);
}

// the main routine
int main(int argc, char **argv) 
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (WINDOW_X, WINDOW_Y);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Rubik's Cube");
    /*glutFullScreen();*/
    init ();
    glutKeyboardFunc (keyboard);
    glutMouseFunc(mouseButton);
    glutPassiveMotionFunc(mouseMove);  // if mouse is moved w/no button */
    glutMotionFunc(mouseMove);
    glutReshapeFunc (reshape);
    glutDisplayFunc (display);
    glutTimerFunc(30, timer, 0);
    glutMainLoop ();
    return 0;
}
