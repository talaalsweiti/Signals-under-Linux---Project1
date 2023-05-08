#include "local.h"

using namespace std;

void createProcesses(char *, int);
void generateRange();
string manageChildrenValues();
void childDeadSignalCatcher(int);
void childValueSignalCatcher(int);
void cleanup();
int processValues(string);

char buffer[BUFSIZ];
using namespace std;
unsigned children[NUM_OF_CHILDREN];
unsigned teamsScore[NUM_OF_TEAMS];
unsigned sigCount = 0;

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

    sigset(SIGUSR1, childValueSignalCatcher);
    sigset(SIGCHLD, childDeadSignalCatcher);
    sigset(SIGINT, childDeadSignalCatcher);

    for (unsigned int i = 0; i < NUM_OF_CHILDREN - 1; i++)
    {
        createProcesses("./child", i);
    }

    if ((mknod(FIFO, S_IFIFO | 0666, 0)) == -1)
    {

        if (unlink(FIFO))
        {
            perror("Error Deleting FIFO");
            exit(4);
        }
        if ((mknod(FIFO, S_IFIFO | 0666, 0)) == -1)
        {
            perror("Error Creating FIFO");
            exit(5);
        }
    }
    createProcesses("./coprocessor", 4);

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

        while (sigCount < NUM_OF_CHILDREN - 1)
        {
            pause();
        }

        cout << "children finished writing\n";
        fflush(stdout);

        string values = manageChildrenValues();

        int winner = processValues(values);
        printf("The winner team for round %d is team %d\n\n\n", r + 1, winner + 1);

        sleep(2);
    }
    int winner = teamsScore[0] > teamsScore[1] ? 0 : 1;
    printf("the overall winner team is team %d\n", winner + 1);

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

    int minValue = rand();
    int maxValue = rand();

    if (minValue > maxValue)
    {
        swap(minValue, maxValue);
    }

    rangeFile << minValue << "," << maxValue;
    cout << "Parent:: Range: " << minValue << "," << maxValue << "\n";

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

string manageChildrenValues()
{

    string values;
    for (unsigned int i = 0; i < NUM_OF_CHILDREN - 1; i++)
    {
        string fileName = "/tmp/" + to_string(children[i]) + ".txt";
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
    cout << "Parent:: values: " << values << "\n";

    close(fifo);

    if (!(fifo = open(FIFO, O_RDONLY)))
    {
        perror(FIFO);
        exit(7);
    }
    memset(buffer, 0x0, BUFSIZ);

    read(fifo, buffer, sizeof(buffer));

    cout << "Parent recieved sums: " << buffer << "\n";

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

    cout << "Parent:: team1 sum: " << teamsValues[0] << "\n";
    cout << "Parent:: team2 sum: " << teamsValues[1] << "\n";
    fflush(stdout);

    sleep(2);
    if (teamsValues[1] > teamsValues[0])
    {
        teamsScore[1]++;
        return 1;
    }
    else
    {
        teamsScore[0]++;
        return 0;
    }
}