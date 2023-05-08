/*
    opgl visualize the game steps. 
    It is informed by the parent at the end of each step using signals and FIFOs.
*/

#include <GL/glut.h>
#include <math.h>

#include "local.h"

#define STRING_SIZE 16
using namespace std;

/* Declare the needed variables */
char *ROUND = new char[STRING_SIZE];
char *TEAM_1_SCORE = new char[STRING_SIZE];
char *TEAM_2_SCORE = new char[STRING_SIZE];
char *WINNER = new char[STRING_SIZE];
char *MIN = new char[STRING_SIZE];
char *MAX = new char[STRING_SIZE];
char *CHILDREN_VALUES[NUM_OF_CHILDREN - 1]; 
char buffer[BUFSIZ];

bool childFlag = false;
bool fileFlag = false;
bool winnerFlag = false;
bool gameEndFlag = false;

unsigned roundNum = 0;
unsigned scores[2] = {0, 0};

/* This function is the SIGUSR1 Signal Handler. It displays the children once they are created */
void childrenView(int signum)
{
    childFlag = true;
    glutPostRedisplay(); /* redisplay when a variable changes */
}

/* This function is the SIGRTMIN+3 Signal Handler. It displays the final winner of the game */
void gameEnd(int signum)
{
    gameEndFlag = true;
    winnerFlag = true;
    int fifo;
    if ((fifo = open(OPENGL_FIFO, O_RDONLY)) == -1)
    {
        perror(OPENGL_FIFO);
        exit(7);
    }

    memset(buffer, 0x0, BUFSIZ);
    read(fifo, buffer, sizeof(buffer));

    close(fifo);
    strcpy(WINNER, buffer);
    glutPostRedisplay();
}

/* This function is the SIGRTMIN Signal Handler. It displays the round number */
void roundBegin(int signum)
{
    roundNum++;
    winnerFlag = false;
    string roundStr = "Round #" + to_string(roundNum);
    strcpy(ROUND, roundStr.c_str());
    glutPostRedisplay();
}

/* this function displays the round winner */
void roundWinner(int signum)
{
    winnerFlag = true;
    int fifo;
    if ((fifo = open(OPENGL_FIFO, O_RDONLY)) == -1) /* open FIFO for reading */
    {
        perror(OPENGL_FIFO);
        exit(7);
    }

    memset(buffer, 0x0, BUFSIZ);
    read(fifo, buffer, sizeof(buffer)); /* read from the FIFO to the buffer */

    close(fifo); /* Close the FIFO */

    /* Display the winner of the round accordingly */
    int winner = stoi(buffer);
    string winnerStr;
    if (winner == -1)
    {
        winnerStr = "Round #" + to_string(roundNum) + ": DRAW, No Winner";
    }
    else
    {
        scores[winner]++;

        winnerStr = "Round #" + to_string(roundNum) + " Winner is Team " + to_string(winner + 1);
    }
    strcpy(WINNER, winnerStr.c_str());
    strcpy(TEAM_1_SCORE, (to_string(scores[0])).c_str());
    strcpy(TEAM_2_SCORE, (to_string(scores[1])).c_str());

    glutPostRedisplay();
}

/* this function reads the string of children values, splits, and displays them */
void readChildrenValues(int signum)
{
    int fifo;
    if ((fifo = open(OPENGL_FIFO, O_RDONLY)) == -1)
    {
        perror(OPENGL_FIFO);
        exit(7);
    }

    memset(buffer, 0x0, BUFSIZ);
    read(fifo, buffer, sizeof(buffer));

    close(fifo);

    stringstream messageStream(buffer);
    cout << buffer << endl;

    unsigned int i = 0;
    while (messageStream.good() && i < NUM_OF_CHILDREN - 1)
    {
        string substr;
        getline(messageStream, substr, ',');
        strcpy(CHILDREN_VALUES[i++], substr.c_str());
    }
    glutPostRedisplay();
}

/* this function reads the range string, splits it and displays the MIN and MAX */
void readRangeValues(int signum)
{
    int fifo;
    if ((fifo = open(OPENGL_FIFO, O_RDONLY)) == -1) /* Open FIFO for read only */
    {
        perror(OPENGL_FIFO);
        exit(1);
    }
    memset(buffer, 0x0, BUFSIZ);
    read(fifo, buffer, sizeof(buffer)); /* read the FIFO content into the buffer */


    close(fifo); /* CLose the FIFO */

    fileFlag = true;

    stringstream messageStream(buffer);

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
    strcpy(MIN, rangeValues[0].c_str());
    strcpy(MAX, rangeValues[1].c_str());

    glutPostRedisplay();
}

/* this function is executed when the window width and/or height are changed */
void reshape(int width, int height)
{
    glViewport(width / 2 - 500, height / 2 - 500, 1000, 1000);
}

/* this function draws a circle with radius r, at position (x,y) */
void drawCircle(float r, float x, float y)
{
    float i = 0.0f;

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(x, y); // Center
    for (i = 0.0f; i <= 360; i++)
        glVertex2f(r * cos(M_PI * i / 180.0) + x, r * sin(M_PI * i / 180.0) + y);

    glEnd();
}

/* display passed text centered at position (cx, cy) */
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

/* draw a rectangle of color(r,g,b) with point coordinates passed */
void drawRectangle(float r, float g, float b, float x1, float y1, float x2, float y2)
{

    glBegin(GL_QUADS); // draw a quad
    glColor3f(r, g, b);
    glVertex2f(x1, y1); // bottom left corner
    glVertex2f(x2, y1); // bottom right corner
    glVertex2f(x2, y2); // top right corner
    glVertex2f(x1, y2); // top left corner
    glEnd();
}

/* display the round # at the top of the window */
void drawRound()
{
    drawRectangle(0.0f, 0.0f, 1.0f, -0.2f, 0.8f, 0.2f, 1.0f);
    displayText(ROUND, 0.0f, 0.9f);
}

/* draw the parent node in the window */
void drawParent()
{
    drawCircle(0.1f, 0.0f, 0.4f);
    displayText("Parent", 0, 0.55f);
}

/* draw the file symbol, name, and min max */
void drawRangeFile()
{
    drawRectangle(0.9725f, 0.8941f, 0.5067f, -0.75f, 0.4f, -0.69f, 0.47f); // TODO: Change color of the file
    drawRectangle(0.9725f, 0.8941f, 0.5067f, -0.73f, 0.42f, -0.67f, 0.49f);
    displayText("Range.txt", -0.71f, 0.34f);
    displayText(MIN, -0.71f, 0.24f);
    displayText(MAX, -0.71f, 0.14f);
}

/* draw the children nodes with numbers, team names and generated names values */
void drawTeams()
{
    drawCircle(0.05f, -0.7f, -0.2f);
    displayText("1", -0.7f, -0.3f);
    displayText(CHILDREN_VALUES[0], -0.7f, -0.1f);
    drawCircle(0.05f, -0.3f, -0.2f);
    displayText("2", -0.3f, -0.3f);
    displayText(CHILDREN_VALUES[1], -0.3f, -0.1f);
    displayText("Team 1", -0.5f, -0.4f);

    drawCircle(0.05f, 0.3f, -0.2f);
    displayText("3", 0.3f, -0.3f);
    displayText(CHILDREN_VALUES[2], 0.3f, -0.1f);
    drawCircle(0.05f, 0.7f, -0.2f);
    displayText("4", 0.7f, -0.3f);
    displayText(CHILDREN_VALUES[3], 0.7f, -0.1f);
    displayText("Team 2", 0.5f, -0.4f);
}

/* draw the coprocessor node */
void drawCoprocessor()
{
    drawCircle(0.05f, 0.6f, 0.4f);
    displayText("Co-processor", 0.6f, 0.3f);
}

/* display the current scores for the teams */
void drawScores()
{
    displayText("Score:", -0.9f, -0.5f);
    displayText(TEAM_1_SCORE, -0.5f, -0.5f);
    displayText(TEAM_2_SCORE, 0.5f, -0.5f);
}

/* display the round winner and the game winner when the game is over */
void drawWinner()
{
    float r = 0.0f, g=0.0f, b=1.0f;
    if(gameEndFlag){
        r = 1.0f;
        b = 0.0f;
    }
    drawRectangle(r, g, b, -0.3f, -1.0f, 0.3f, -0.8f);
    displayText(WINNER, 0.0f, -0.9f);
}

/* main function for displaying the window components */
void display()
{
    // reshape();

    // Set every pixel in the frame buffer to the current clear color.
    glClear(GL_COLOR_BUFFER_BIT);

    drawRound();

    drawParent();
    if (fileFlag)
    {
        drawRangeFile();
    }

    if (childFlag)
    {
        drawTeams();
        drawCoprocessor();
        drawScores();
    }

    if (winnerFlag)
    {
        drawWinner();
    }

    // Flush drawing command buffer to make drawing happen asap.
    glFlush();
}

int main(int argc, char **argv)
{
    /* initialize variables */
    strcpy(TEAM_1_SCORE, "0");
    strcpy(TEAM_2_SCORE, "0");
    for (int i = 0; i < NUM_OF_CHILDREN - 1; i++)
    {
        CHILDREN_VALUES[i] = new char[STRING_SIZE];
        CHILDREN_VALUES[i][0] = 0x0;
    }

    /* this function sets all the catchable signals used for the opgl */
    if (sigset(SIGUSR1, childrenView) == SIG_ERR)
    {
        perror("SIGUSR1 handler");
        exit(1);
    }
    if (sigset(SIGUSR2, readRangeValues) == SIG_ERR)
    {
        perror("SIGUSR2 handler");
        exit(2);
    }
    if (sigset(SIGRTMIN + 1, readChildrenValues) == SIG_ERR)
    {
        perror("SIGRTMIN+1 handler");
        exit(2);
    }
    if (sigset(SIGRTMIN + 2, roundWinner) == SIG_ERR)
    {
        perror("SIGRTMIN+2 handler");
        exit(2);
    }
    if (sigset(SIGRTMIN + 3, gameEnd) == SIG_ERR)
    {
        perror("SIGRTMIN+3 handler");
        exit(2);
    }
    if (sigset(SIGRTMIN, roundBegin) == SIG_ERR)
    {
        perror("SIGRTMIN handler");
        exit(2);
    }

    glutInit(&argc, argv);

    // The image is not animated so single buffering is OK.
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    // Window position (from top corner), and size (width and hieght)
    glutInitWindowPosition(400, 30);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("Project#1");

    // call this whenever window needs redrawing
    glutDisplayFunc(display);

    glutReshapeFunc(reshape);

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    return (0); // This line is never reached.
}
