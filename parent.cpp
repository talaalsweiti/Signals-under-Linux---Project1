/*
1190114 - Aseel Sabri
1191068 - Tala AlSweiti
1191448 - Shahd AbuDaghash
*/

/*
    This is the parent class. It is responsible for the creation and coordination
    between the children to assure the success of the game.
*/
#include "local.h"

using namespace std;

/* Defining the prototype of the function used */
void addSignalCatchers();
void createProcesses(char *, int);
void generateRange();
string manageChildrenValues();
void childDeadSignalCatcher(int);
void childValueSignalCatcher(int);
void cleanup();
int processValues(string);
void createOpenglProcess();
void createFifo(const char *);
void sendToOpgl(string);

/* define constants used */
char buffer[BUFSIZ];
unsigned children[NUM_OF_CHILDREN];
pid_t opglPid;
unsigned teamsScore[NUM_OF_TEAMS];
unsigned sigCount = 0; /* count the signals received from the children */
unsigned rangeValues[2];

int main(int argc, char *argv[])
{
    unsigned rounds = 5; /* Assume default number is 5 */
    if (argc > 2) /* error when having more than two arguments */
    {
        printf("Usage: %s [num_of_rounds]\n", argv[0]);
        exit(1);
    }
    if (argc == 2) /* # of rounds passed */
    {
        /* check if passed argument is a positive integer number */
        for (unsigned int i = 0; i < strlen(argv[1]); i++)
        {
            if (!isdigit(argv[1][i]))
            {
                printf("num_of_round must be a positive integer\n");
                exit(2);
            }
        }

        rounds = atoi(argv[1]); /* convert passed argument to an integer */

        if (rounds < 1) /* handle when passed # of rounds = 0 */
        {
            printf("num_of_round must be a positive integer\n");
            exit(3);
        }
    }

    addSignalCatchers(); // add the signal catchers used

    /* create FIFO for communication with coprocessor and OPENGL_FIFO for communication with opgl */
    createFifo(FIFO);
    createFifo(OPENGL_FIFO);

    /* fork an openGL child */
    createOpenglProcess();

    sleep(2); /* sleep 2 seconds after opening the display */

    /* fork NUM_OF_CHILDREN children processes */
    for (unsigned int i = 0; i < NUM_OF_CHILDREN - 1; i++)
    {
        createProcesses("./child", i);
    }

    /* fork the coprocessor child  */
    createProcesses("./coprocessor", 4);

    /* send SIGUSR1 signal to display the children */
    kill(opglPid, SIGUSR1);

    srand((unsigned)getpid()); /* Set parent PID as unique seed for rand */

    sleep(2);
    for (unsigned int r = 0; r < rounds; r++)
    {
        kill(opglPid, SIGRTMIN); /* send signal to opgl to view the beginning of a round */
        sigCount = 0;
        generateRange(); /* generate MIN and MAX */

        /* send signal to children to generate their number and send it back here */
        for (unsigned int i = 0; i < NUM_OF_CHILDREN - 1; i++)
        {
            kill(children[i], SIGUSR1);
            sleep(1);
        }

        kill(opglPid, SIGUSR2); /* display range values in opgl */
        sendToOpgl(to_string(rangeValues[0]) + "," + to_string(rangeValues[1])); /* send MIN and MAX to opgl */

        /* Suspend parent until all children have responded with their generated values */
        while (sigCount < NUM_OF_CHILDREN - 1) 
        {
            pause();
        }

        cout << "children finished writing" << endl;

        string values = manageChildrenValues(); /* concatenate all values in one string */

        sleep(1); /* for display */

        kill(opglPid, SIGRTMIN + 1); /* send signal to display the children values */
        sendToOpgl(values); /* send the values to opgl through FIFO */

        int winner = processValues(values); /* process values and return round winner */

        kill(opglPid, SIGRTMIN + 2); /* send signal to opgl to display round winner of the round */
        sendToOpgl(to_string(winner)); /* send winner string */

        if (winner == -1)
        {
            printf("DRAW, No winner in this round\n");
        }
        else
        {
            printf("The winner team for round %d is team %d\n\n\n", r + 1, winner + 1);
        }

        sleep(2);
    }

    /* Decide the game winner after rounds are finished */
    string msg;
    if (teamsScore[0] > teamsScore[1])
    {
        msg = "Winner is Team 1";
    }
    else if (teamsScore[0] < teamsScore[1])
    {
        msg = "Winner is Team 2";
    }
    else
    {
        msg = "DRAW, No Winner";
    }
    cout << msg << endl;
    kill(opglPid, SIGRTMIN + 3); /* send a signal to child to display the winner of the game */
    sendToOpgl(msg);

    cleanup();
}

/* this function sets all the catchable signals used for the parent */
void addSignalCatchers(){
    if (sigset(SIGUSR1, childValueSignalCatcher) == SIG_ERR) /* child sends SIGUSR1 when finished writing */
    {
        perror("SIGUSR1 handler");
        exit(9);
    }
    if (sigset(SIGCHLD, childDeadSignalCatcher) == SIG_ERR) /* child sends SIGCHLD when failed */
    {
        perror("SIGCHLD handler");
        exit(10);
    }
    if (sigset(SIGINT, childDeadSignalCatcher) == SIG_ERR) /* child is interrupted by SIGINT */
    {
        perror("SIGINT handler");
        exit(11);
    }
}

/* callback function for SIGCHLD and SIGINT, cleanup and exit if a child fails or gets interrupted by SIGINT */
void childDeadSignalCatcher(int signum)
{
    cout << "A child failed\n";
    cleanup();
    exit(signum);
}

/*  
    Callback function for SIGUSR1, increments the number of childs that sent the SIGUSR1 signals
    indicating the finished writing
*/
void childValueSignalCatcher(int signum)
{
    sigCount++;
}

/* This function forks a child for the parent that executes the the content of the passed filename */
void createProcesses(char *file, int i)
{
    pid_t pid = fork(); /* fork an opengl child process */
    switch (pid)
    {
    case -1: /* error performing fork */
        perror("Fork");
        exit(6);

    case 0: /* In the child */
        execlp(file, file, (char *)NULL); /* execute passed file */
        perror("exec failure ");
        exit(7);
        break;

    default: /* In the parent */
        children[i] = pid; /* save the PID of the child */
    }
}

/* This function create a child for the parent that is responsible for the display using OpenGL */
void createOpenglProcess()
{
    pid_t pid = fork(); /* fork an opengl child process */
    switch (pid)
    {
    case -1: /* error performing fork */
        perror("Fork");
        exit(6);

    case 0: /* In the opgl child */
        execlp("./opgl", "./opgl", (char *)NULL); /* execute opgl.cpp file */
        perror("exec failure ");
        exit(7);
        break;

    default: /* In the parent */
        opglPid = pid; /* save the pid of the opgl child */
    }
}

/* This function generates two numbers MIN and MAX and saves them in range.txt */
void generateRange()
{
    ofstream rangeFile;
    rangeFile.open("/tmp/range.txt");

    rangeValues[0] = rand();
    rangeValues[1] = rand();

    if (rangeValues[0] > rangeValues[1])
    {
        swap(rangeValues[0], rangeValues[1]);
    }

    rangeFile << rangeValues[0] << "," << rangeValues[1];

    cout << "Parent:: Range: " << rangeValues[0] << "," << rangeValues[1] << "\n";

    rangeFile.close();
}

/* 
    This function writes the values the children generated, each in its corresponding file.
    It also returns all the values concatenated in a string seperated by a coma
*/
string manageChildrenValues()
{

    string values;
    string fileName;
    for (unsigned int i = 0; i < NUM_OF_CHILDREN - 1; i++)
    {
        fileName = "/tmp/" + to_string(children[i]) + ".txt";
        ifstream childFile(fileName);
        if (!childFile.good())
        {
            perror("Open child file");
            exit(8);
        }

        string line;
        getline(childFile, line);
        values += line;
        if (i < 3)
        {
            values += ",";
        }
    }
    return values;
}

/* send values to the coprocessor to process them and return the sums */
int processValues(string values)
{
    int fifo;
    if (!(fifo = open(FIFO, O_WRONLY))) /* open the FIFO for writing */
    {
        perror(FIFO);
        exit(7);
    }

    write(fifo, values.c_str(), values.length() + 1); /* write the children values to the FIFO */
    cout << "Parent:: values: " << values << endl;

    close(fifo); /* close the FIFO to open it later for reading */

    if (!(fifo = open(FIFO, O_RDONLY))) /* open FIFO for reading */
    {
        perror(FIFO);
        exit(7);
    }
    memset(buffer, 0x0, BUFSIZ);

    read(fifo, buffer, sizeof(buffer)); /* Read the FIFO to buffer */

    cout << "Parent recieved sums: " << buffer << endl;

    close(fifo); /* close the pipe */

    /* split the received string into the two sum values */
    stringstream messageStream(buffer);
    memset(buffer, 0x0, BUFSIZ);

    unsigned int i = 0;
    double teamsValues[NUM_OF_TEAMS];

    while (messageStream.good() && i < NUM_OF_TEAMS)
    {
        string substr;
        getline(messageStream, substr, ',');
        teamsValues[i++] = stod(substr);
    }

    cout << "Parent:: team1 sum: " << teamsValues[0] << endl;
    cout << "Parent:: team2 sum: " << teamsValues[1] << endl;

    sleep(2);
    /* Decide the winner of the round */
    if (teamsValues[1] > teamsValues[0])
    {
        teamsScore[1]++;
        return 1;
    }
    else if (teamsValues[1] < teamsValues[0])
    {

        teamsScore[0]++;
        return 0;
    }
    return -1;
}

/* This function opens a FIFO and sends the passed message to opgl */
void sendToOpgl(string message)
{
    int fifo;
    if (!(fifo = open(OPENGL_FIFO, O_WRONLY)))
    {
        perror(OPENGL_FIFO);
        exit(12);
    }
    write(fifo, message.c_str(), message.length() + 1); /* write message to FIFO */
    close(fifo);
}

/* This function creates a FIFO with the specified name */
void createFifo(const char *fifoName)
{
    if ((mknod(fifoName, S_IFIFO | 0666, 0)) == -1) /* Try to make the FIFO */
    {
        /* if mknod fails, try unlinking FIFO and try mknod again */
        if (unlink(fifoName)) 
        {
            perror("Error Deleting FIFO");
            exit(4);
        }
        if ((mknod(fifoName, S_IFIFO | 0666, 0)) == -1) 
        {
            /* If mknod error persists, exit the program */
            perror("Error Creating FIFO");
            exit(5); 
        }
    }
}

/* This function cleans up used respurces when game is finished or exited for some reason*/
void cleanup() 
{
    sigset(SIGCHLD, SIG_DFL); /* stop taking signal from failed children */

    for (auto child : children) /* kill all children */
    {
        kill(child, SIGKILL);
    }
    string fileName;

    /* delete files where children values are saved */
    for (unsigned int i = 0; i < NUM_OF_CHILDREN - 1; i++) 
    {
        fileName = "/tmp/" + to_string(children[i]) + ".txt";
        if (unlink(fileName.c_str()))
        {
            cout << "Unable to delete " << fileName << " , file does not exists" << endl;
        }
    }

    /* delete range.txt file */
    fileName = "/tmp/range.txt";
    if (unlink(fileName.c_str()))
    {
        cout << "Unable to delete " << fileName << " , file does not exists" << endl;
    }

    /* unlink coprocessor FIFO */
    if (unlink(FIFO))
    {
        perror("Error Deleting FIFO");
        exit(4);
    }
    exit(0);
}