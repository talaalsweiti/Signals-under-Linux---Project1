#include "local.h"

using namespace std;

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

char buffer[BUFSIZ];
using namespace std;
unsigned children[NUM_OF_CHILDREN];
pid_t opglPid;
unsigned teamsScore[NUM_OF_TEAMS];
unsigned sigCount = 0;
unsigned rangeValues[2];

int main(int argc, char *argv[])
{
    unsigned rounds = 5;
    if (argc > 2)
    {
        printf("Usage: %s [num_of_rounds]\n", argv[0]);
        exit(1);
    }
    if (argc == 2)
    {
        for (unsigned int i = 0; i < strlen(argv[1]); i++)
        {
            if (!isdigit(argv[1][i]))
            {
                printf("num_of_round must be a positive integer\n");
                exit(2);
            }
        }
        rounds = atoi(argv[1]);
        if (rounds < 1)
        {
            printf("num_of_round must be a positive integer\n");
            exit(3);
        }
    }

    if (sigset(SIGUSR1, childValueSignalCatcher) == SIG_ERR)
    {
        perror("SIGUSR1 handler");
        exit(9);
    }
    if (sigset(SIGCHLD, childDeadSignalCatcher) == SIG_ERR)
    {
        perror("SIGCHLD handler");
        exit(10);
    }
    if (sigset(SIGINT, childDeadSignalCatcher) == SIG_ERR)
    {
        perror("SIGINT handler");
        exit(11);
    }

    createFifo(FIFO);
    createFifo(OPENGL_FIFO);

    createOpenglProcess();

    sleep(2); // ???

    for (unsigned int i = 0; i < NUM_OF_CHILDREN - 1; i++)
    {
        createProcesses("./child", i);
    }

    createProcesses("./coprocessor", 4);

    kill(opglPid, SIGUSR1);

    srand((unsigned)getpid());

    sleep(2);
    for (unsigned int r = 0; r < rounds; r++)
    {
        sigCount = 0;
        generateRange();

        for (unsigned int i = 0; i < NUM_OF_CHILDREN - 1; i++)
        {
            kill(children[i], SIGUSR1);
            sleep(1);
        }

        kill(opglPid, SIGUSR2);
        sendToOpgl(to_string(rangeValues[0]) + "," + to_string(rangeValues[1]));
        // sleep(3);

        while (sigCount < NUM_OF_CHILDREN - 1)
        {
            pause();
        }

        cout << "children finished writing" << endl;

        string values = manageChildrenValues();

        int winner = processValues(values);
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
    if (teamsScore[0] > teamsScore[1])
    {
        printf("The overall winner team is team 1\n");
    }
    else if (teamsScore[0] < teamsScore[1])
    {
        printf("The overall winner team is team 2\n");
    }
    else
    {
        printf("DRAW, between the two teams\n");
    }

    cleanup();
}

void createProcesses(char *file, int i)
{
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        perror("Fork");
        exit(6);

    case 0: /* In the child */
        execlp(file, file, (char *)NULL);
        perror("exec failure ");
        exit(7);
        break;

    default: /* In the parent */
        children[i] = pid;
    }
}

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

void childDeadSignalCatcher(int signum)
{
    cout << "A child failed\n";
    cleanup();
    exit(signum);
}

void childValueSignalCatcher(int signum)
{
    sigCount++;
}

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

int processValues(string values)
{
    int fifo;
    if (!(fifo = open(FIFO, O_WRONLY)))
    {
        perror(FIFO);
        exit(7);
    }

    write(fifo, values.c_str(), values.length() + 1);
    cout << "Parent:: values: " << values << endl;

    close(fifo);

    if (!(fifo = open(FIFO, O_RDONLY)))
    {
        perror(FIFO);
        exit(7);
    }
    memset(buffer, 0x0, BUFSIZ);

    read(fifo, buffer, sizeof(buffer));

    cout << "Parent recieved sums: " << buffer << endl;

    close(fifo);

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

void cleanup()
{
    sigset(SIGCHLD, SIG_DFL);

    for (auto child : children)
    {
        kill(child, SIGKILL);
    }
    string fileName;
    for (unsigned int i = 0; i < NUM_OF_CHILDREN - 1; i++)
    {
        fileName = "/tmp/" + to_string(children[i]) + ".txt";
        if (unlink(fileName.c_str()))
        {
            cout << "Unable to delete " << fileName << " , file does not exists" << endl;
        }
    }
    fileName = "/tmp/range.txt";
    if (unlink(fileName.c_str()))
    {
        cout << "Unable to delete " << fileName << " , file does not exists" << endl;
    }
    if (unlink(FIFO))
    {
        perror("Error Deleting FIFO");
        exit(4);
    }
    exit(0);
}

void createOpenglProcess()
{
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        perror("Fork");
        exit(6);

    case 0: /* In the child */
        execlp("./opgl", "./opgl", (char *)NULL);
        perror("exec failure ");
        exit(7);
        break;

    default: /* In the parent */
        opglPid = pid;
    }
}

void sendToOpgl(string message)
{
    int fifo;
    if (!(fifo = open(OPENGL_FIFO, O_WRONLY)))
    {
        perror(OPENGL_FIFO);
        exit(12);
    }
    write(fifo, message.c_str(), message.length() + 1);
    close(fifo);
}

void createFifo(const char *fifoName)
{
    if ((mknod(fifoName, S_IFIFO | 0666, 0)) == -1)
    {

        if (unlink(fifoName))
        {
            perror("Error Deleting FIFO");
            exit(4);
        }
        if ((mknod(fifoName, S_IFIFO | 0666, 0)) == -1)
        {
            perror("Error Creating FIFO");
            exit(5);
        }
    }
}