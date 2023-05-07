#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include <string>

// function to reshape the window
void reshape()
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
}

void drawCircle(float r, float x, float y)
{

    float i = 0.0f;

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(x, y); // Center
    for (i = 0.0f; i <= 360; i++)
        glVertex2f(r * cos(M_PI * i / 180.0) + x, r * sin(M_PI * i / 180.0) + y);

    glEnd();
}

void displayText(char* text, float cx, float cy)
{
    // Set the color to white
    glColor3f(1.0f, 1.0f, 1.0f);

    int text_width = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char *)text);
    int text_height = 24; // bitmap height for GLUT_BITMAP_TIMES_ROMAN_24

    float x = cx - ((float)text_width /  (float)glutGet(GLUT_WINDOW_WIDTH));
    float y = cy - ((float)text_height / 2.0f / (float)glutGet(GLUT_WINDOW_HEIGHT));

    // Set the text position
    glRasterPos2f(x, y);

    // Draw the text character by character
    for (int i = 0; i < strlen(text); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
    }
}

// void displayText2(char* text, float cx, float cy)
// {
//     // Set the color to white
//     glColor3f(1.0f, 1.0f, 1.0f);

//     int text_width = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char *)text.c_str());
//     int text_height = 24; // bitmap height for GLUT_BITMAP_TIMES_ROMAN_24

//     float x = cx - ((float)text_width /  (float)glutGet(GLUT_WINDOW_WIDTH));
//     float y = cy - ((float)text_height / 2.0f / (float)glutGet(GLUT_WINDOW_HEIGHT));

//     // Set the text position
//     glRasterPos2f(x, y);

//     // Draw the text character by character
//     for (int i = 0; i < text.length(); i++)
//     {
//         glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
//     }
    
//     currentText = text; // store current text for redrawing
//     glutPostRedisplay(); // mark window as needing to be redrawn
// }

void display()
{
    reshape();

    // Set every pixel in the frame buffer to the current clear color.
    glClear(GL_COLOR_BUFFER_BIT);

    // draw round number
    displayText("Round #1", 0.0f, 0.8f);

    // draw parent
    drawCircle(0.1f, 0.0f, 0.4f);
    displayText("Parent", 0, 0.55f);
    // displayText2("Child", 0, 0.55f);

    // draw 4 children
    drawCircle(0.05f, -0.7f, -0.3f);
    displayText("1", -0.7f, -0.4f);
    drawCircle(0.05f, -0.3f, -0.3f);
    displayText("2", -0.3f, -0.4f);
    drawCircle(0.05f, 0.3f, -0.3f);
    displayText("3", 0.3f, -0.4f);
    drawCircle(0.05f, 0.7f, -0.3f);
    displayText("4", 0.7f, -0.4f);
 
    // draw coprocessor
    drawCircle(0.05f, 0.6f, 0.4f);
    displayText("coprocessor", 0.6f, 0.3f);

    // Flush drawing command buffer to make drawing happen asap.
    glFlush();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    // The image is not animated so single buffering is OK.
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    // Window position (from top corner), and size (width and hieght)
    glutInitWindowPosition(350, 70);
    glutInitWindowSize(1200, 900);
    glutCreateWindow("test");

    // Initialize OpenGL as we like it..
    // glEnable ( GL_DEPTH_TEST );

    // call this whenever window needs redrawing
    glutDisplayFunc(display);

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    return (0); // This line is never reached.
}