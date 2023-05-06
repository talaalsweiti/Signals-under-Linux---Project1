#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>

void drawCircle(float r, float x, float y)
{
	
	float i = 0.0f;

	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(x, y); // Center
	for (i = 0.0f; i <= 360; i++)
		glVertex2f(r * cos(M_PI * i / 180.0) + x, r * sin(M_PI * i / 180.0) + y);

	glEnd();
}

void displayText(float x, float y){
	// Set the color to white
    glColor3f(1.0f, 1.0f, 1.0f);

    // Set the text position
    glRasterPos2f(-0.5f, 0.5f);

    // Draw the text character by character
    char* text = "Hello, world!";
    for (int i = 0; i < strlen(text); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, text[i]);
    }
}

void display()
{

	// Set the viewport to match the aspect ratio of the window
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(0, 0, width, height);

    // Calculate the aspect ratio of the window
    float aspect_ratio = (float)width / (float)height;

    // Set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-aspect_ratio, aspect_ratio, -1.0f, 1.0f);

    // Switch back to the modelview matrix for drawing
    glMatrixMode(GL_MODELVIEW);

	// Set every pixel in the frame buffer to the current clear color.
	glClear(GL_COLOR_BUFFER_BIT);

	// Drawing is done by specifying a sequence of vertices.  The way these
	// vertices are connected (or not connected) depends on the argument to
	// glBegin.  GL_POLYGON constructs a filled polygon.
	//   glBegin(GL_POLYGON);
	//     glColor3f(1, 0, 0); glVertex3f(-0.6, -0.75, 0.5);
	//     glColor3f(0, 1, 0); glVertex3f(0.6, -0.75, 0);
	//     glColor3f(0, 0, 1); glVertex3f(0, 0.75, 0);
	//   glEnd();

	// glBegin(GL_TRIANGLES);
	// glVertex3f(-0.6, -0.75, 0.5);
	// glVertex3f(0.6, -0.75, 0);
	// glVertex3f(0, 0.75, 0);

	// glEnd();

	drawCircle(0.05f, 0.0f, 0.5f);
	drawCircle(0.031f, 0.3f, 0.5f);
	displayText(0,0);

	// Flush drawing command buffer to make drawing happen as soon as possible.
	glFlush();
}



int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	// The image is not animated so single buffering is OK.
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	// Window position (from top corner), and size (width and hieght)
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(400, 400);
	glutCreateWindow("test");

	// Initialize OpenGL as we like it..
	// glEnable ( GL_DEPTH_TEST );

	// call this whenever window needs redrawing
	glutDisplayFunc(display);

	// Start the main loop.  glutMainLoop never returns.
	glutMainLoop();

	return (0); // This line is never reached.
}