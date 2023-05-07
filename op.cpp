#include <GL/glut.h>
#include <stdlib.h>
#include <GL/freeglut.h>
#include <iostream>
#include <GL/glut.h>

/*
// Window size
int windowWidth = 800;
int windowHeight = 600;

// Mouse position
int mouseX = 0;
int mouseY = 0;

// Text to be displayed
const char* text = "Text for rectangle";

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw rectangle
    glColor3f(0.5f, 0.5f, 0.5f);
    glRecti(100, 100, 400, 400);

    // Draw text window
    glColor3f(1.0f, 1.0f, 1.0f);
    glRecti(mouseX, windowHeight - mouseY, mouseX + 150, windowHeight - mouseY - 50);
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(mouseX + 10, windowHeight - mouseY - 30);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)text);

    glFlush();
}

void mouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouseX = x;
        mouseY = y;

        glutPostRedisplay();
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("OpenGL Test");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
    glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(display);
    glutMouseFunc(mouseClick);

    glutMainLoop();
    return 0;
}
*/

#include <iostream>
#include <GL/glut.h>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

int mainWindow, textWindow;

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glVertex2i(50, 50);
        glVertex2i(200, 50);
        glVertex2i(200, 200);
        glVertex2i(50, 200);
    glEnd();
    glFlush();
}

void textDisplay() {
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(100, 100);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"Text for rectangle");
    glFlush();
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int windowID = glutCreateWindow("Text Window");
        glutSetWindow(windowID);
        glutDisplayFunc(textDisplay);
        glutMainLoop();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    mainWindow = glutCreateWindow("Main Window");
    glutDisplayFunc(display);
    glutMouseFunc(mouseClick);
    init();
    glutMainLoop();
    return 0;
}
