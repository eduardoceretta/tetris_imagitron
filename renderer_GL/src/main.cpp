/**
 *	Eduardo Ceretta Dalla Favera
 *  eduardo.ceretta@gmail.com
 *  May 2011
 *
 *  Application's Main code.
 *  Create the application and bind the GLUT callbacks.
 */
#include <iostream>
#include <GL\glew.h>
#include <GL\glut.h>

#include "main.h"

#include "app.h"
#include "GLUtils/GLError.h"

App app;

int main(int argc, char *argv[]){
  app.loadParameters(argc, argv);
  app.initGL(&argc, argv);
  app.loadResources();

  setGlutCallbacks();
  glutMainLoop();
}


void setGlutCallbacks(){
  glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecial);
	glutMotionFunc(mouseMove);
	glutMouseFunc(mouseClick);
	glutReshapeFunc(reshape);
}

void reshape(int w, int h){
  app.listenReshape(w, h);
}

void keyboardSpecial(int key, int x, int y){
  app.listenKeyboardSpecial(key);
}

void keyboard(unsigned char key, int x, int y){
  app.listenKeyboard(key);
}

void mouseClick(int button, int state, int x, int y){
  app.listenMouseClick(button, state, x, y);
}

void mouseMove(int x, int y){
  app.listenMouseMove(x, y);
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  app.render();

#ifndef NDEBUG  
  PrintGLError();
#endif  

  glutSwapBuffers();
}
/**/