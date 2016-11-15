#include "GL/glut.h"
#include "util.h"

void display(void) 
{
    glClear(GL_COLOR_BUFFER_BIT);
    utilityCentral(Event());
    glutSwapBuffers();
}

void keyboard(const byte key, const int x, const int y) 
{
    utilityCentral(Event(key,x,y));
}

void mouseMotion(const int x, const int y) 
{
    utilityCentral(Event(MOUSEDRAG,x,y));
}

void mouseClick(const int button, const int state, const int x, const int y) 
{
    utilityCentral(Event(button,state,x,y));
}

void reshape(const int w, const int h) 
{
    glLoadIdentity();           // initialize transformation matrix
    gluOrtho2D(0.0,w,0.0,h);
    glViewport(0,0,w,h);        // adjust viewport to new window
    utilityCentral(Event(RESHAPE, w, h));
    glutPostRedisplay();
}

void ticktock(int value)
{
   utilityCentral(Event(TICKTOCK));
   glutTimerFunc(value, ticktock, value);
}
