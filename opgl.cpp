#include <GL/glut.h>
#include <math.h>

#include "local.h"

using namespace std;

char *ROUND = "Round #1";
char *TEAM_1_SCORE = "0";
char *TEAM_2_SCORE = "0";
char *WINNER = "The Winner is TEAM 1";
char *MIN;
char *MAX;
char buffer[BUFSIZ];

bool child = false;
bool file = false;

void childrenView(int signum)
{
    child = true;
    glutPostRedisplay();
}

void readRangeFile(int signum)
{
    int fifo;
    if ((fifo = open(OPENGL_FIFO, O_RDONLY)) == -1)
    {
        perror(OPENGL_FIFO);
        exit(1);
    }
    memset(buffer, 0x0, BUFSIZ);
    read(fifo, buffer, sizeof(buffer));

    stringstream messageStream(buffer);


    close(fifo);
    file = true;

    vector<string> rangeValues(2);
    unsigned int i = 0;


    while (messageStream.good())
    {
        string substr;
        getline(messageStream, substr, ',');
        rangeValues[i++] = substr;
    }
    rangeValues[0] = "MIN = " + rangeValues[0];
    rangeValues[1] = "MAX = " + rangeValues[1];
    MIN = new char[rangeValues[0].length() + 1];
    MAX = new char[rangeValues[1].length() + 1];
    strcpy(MIN, rangeValues[0].c_str());
    strcpy(MAX, rangeValues[1].c_str());
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    // Keep the viewport size fixed at 800x600 pixels
    glViewport(width / 2 - 500, height / 2 - 500, 1000, 1000);
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

void displayText(char *text, float cx, float cy)
{
    // Set the color to white
    glColor3f(1.0f, 1.0f, 1.0f);

    int text_width = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char *)text);
    int text_height = 24; // bitmap height for GLUT_BITMAP_TIMES_ROMAN_24

    float x = cx - ((float)text_width / (float)1000);
    float y = cy - ((float)text_height / 2.0f / (float)glutGet(GLUT_WINDOW_HEIGHT));

    // Set the text position
    glRasterPos2f(x, y);

    // Draw the text character by character
    for (int i = 0; i < strlen(text); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
    }
}

void drawRectangle(float x1, float y1, float x2, float y2)
{

    glBegin(GL_QUADS); // draw a quad
    glColor3f(0.0f, 0.0f, 1.0f);
    // -0.2f, -1.0f, 0.2f, -1.0f
    glVertex2f(x1, y1); // bottom left corner
    glVertex2f(x2, y1); // bottom right corner
    glVertex2f(x2, y2); // top right corner
    glVertex2f(x1, y2); // top left corner
    glEnd();
}

void drawRound()
{
    drawRectangle(-0.2f, 0.8f, 0.2f, 1.0f);
    displayText(ROUND, 0.0f, 0.9f);
}

void drawParent()
{
    drawCircle(0.1f, 0.0f, 0.4f);
    displayText("Parent", 0, 0.55f);
}

void drawRangeFile()
{
    drawRectangle(-0.75f, 0.4f, -0.69f, 0.47f); // TODO: Change color of the file
    drawRectangle(-0.73f, 0.42f, -0.67f, 0.49f);
    displayText("Range.txt", -0.71f, 0.34f);
    displayText(MIN, -0.71f, 0.24f);
    displayText(MAX, -0.71f, 0.14f);
}

void drawTeams()
{
    drawCircle(0.05f, -0.7f, -0.2f);
    displayText("1", -0.7f, -0.3f);
    drawCircle(0.05f, -0.3f, -0.2f);
    displayText("2", -0.3f, -0.3f);
    displayText("Team 1", -0.5f, -0.4f);

    drawCircle(0.05f, 0.3f, -0.2f);
    displayText("3", 0.3f, -0.3f);
    drawCircle(0.05f, 0.7f, -0.2f);
    displayText("4", 0.7f, -0.3f);
    displayText("Team 2", 0.5f, -0.4f);
}

void drawCoprocessor()
{
    drawCircle(0.05f, 0.6f, 0.4f);
    displayText("Co-processor", 0.6f, 0.3f);
}

void drawScores()
{
    displayText("Score:", -0.9f, -0.5f);
    displayText(TEAM_1_SCORE, -0.5f, -0.5f);
    displayText(TEAM_2_SCORE, 0.5f, -0.5f);
}

void drawWinner()
{
    drawRectangle(-0.3f, -1.0f, 0.3f, -0.8f);
    displayText(WINNER, 0.0f, -0.9f);
}

void redraw()
{

    glClear(GL_COLOR_BUFFER_BIT);

    drawRound();

    drawParent();

    drawRangeFile();

    drawTeams();

    drawCoprocessor();

    drawScores();

    drawWinner();
}

void display()
{
    // reshape();

    // Set every pixel in the frame buffer to the current clear color.
    glClear(GL_COLOR_BUFFER_BIT);

    drawRound();

    drawParent();
    if (file)
    {
        drawRangeFile();
    }

    if (child)
    {
        drawTeams();
        drawCoprocessor();
        drawScores();
    }

    drawWinner();

    // Flush drawing command buffer to make drawing happen asap.
    glFlush();
}

int main(int argc, char **argv)
{
    sigset(SIGUSR1, childrenView);
    sigset(SIGUSR2, readRangeFile);

    glutInit(&argc, argv);

    // The image is not animated so single buffering is OK.
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    // Window position (from top corner), and size (width and hieght)
    glutInitWindowPosition(400, 30);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("Project#1");

    // Initialize OpenGL as we like it..
    // glEnable ( GL_DEPTH_TEST );

    // call this whenever window needs redrawing
    glutDisplayFunc(display);

    glutReshapeFunc(reshape);

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    return (0); // This line is never reached.
}
