#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <GL/glut.h>
#include "util.h"
#include "game.h"
#include "callbacks.h"

using namespace std;

int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      cout << "Usage : " << argv[0] << " playfield.in\n";
      return 1;
   }
   else
   {
      glutInit(&argc, argv);
      int msec = 2000;
      if (argc > 2) msec = atoi(argv[2]);
      initOpenGL(argv[1], msec, 1024, 800);

      glutMainLoop();
   }
   return 0;
}

void initOpenGL(char *name, int msec, const int rows, const int cols)
{
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
   
   glutInitWindowSize(rows, cols);

   glutInitWindowPosition(50,50);      // initial window  position
   glutCreateWindow(name);      // window title

   utilityCentral(Event(name));

   glClearColor(0,0,0,0);              // use black for glClear command
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);
   glMatrixMode(GL_PROJECTION);

   glutDisplayFunc(display);           // how to redisplay window
   glutKeyboardFunc(keyboard);         // how to handle key presses
   glutMotionFunc(mouseMotion);        // how to handle mouse movements
   glutMouseFunc(mouseClick);          // how to handle mouse clicks
   glutReshapeFunc(reshape);           // how to resize window
   glutTimerFunc(msec, ticktock, msec);
}

