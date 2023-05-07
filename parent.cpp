#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <limits.h>
#include <sstream>

#define FIFO "/tmp/FIFO"

using namespace std;

void createProcesses(char *, int);
void generateRange();
string manageChildrenValues();
void childDeadSignalCatcher(int);
void childValueSignalCatcher(int);
void cleanup();
int processValues(string);

char buffer[1024];
using namespace std;
unsigned children[5];
unsigned teamsScore[2];

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

    for (int i = 0; i < 4; i++)
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

        for (int i = 0; i < 4; i++)
        {
            kill(children[i], SIGUSR1);
            sleep(1);
        }

        while (sigCount < 4)
        {
            pause();
        }

        cout << "children finished writing\n";
        fflush(stdout);

        string values = manageChildrenValues();

        int winner = processValues(values);
        printf("the winner team for round %d is team %d\n\n\n", r + 1, winner + 1);

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

void childDeadSignalCatcher(int theSig)
{
    cout << "A child failed\n";
    cleanup();
    exit(theSig);
}

void childValueSignalCatcher(int theSig)
{
    sigCount++;
    // cout << "A child finished writing\n";
    // fflush(stdout);
}

void cleanup()
{
    sigset(SIGCHLD, SIG_DFL);

    for (auto child : children)
    {
        kill(child, SIGKILL);
    }

    for (int i = 0; i < 4; i++)
    {
        string fileName = "/tmp/" + to_string(children[i]) + ".txt";
        int result = unlink(fileName.c_str());
        // if(!result){
        //     // if was not successfull do sth
        //     // if the program terminated before creating the file?
        // }
    }
    string fileName = "/tmp/range.txt";
    int result = unlink(fileName.c_str());
    exit(0);
}

string manageChildrenValues()
{

    string values;
    for (int i = 0; i < 4; i++)
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
    // cout << values << "\n";
    return values;
    // send values to coprocessor via pipe
}

int processValues(string values)
{
    int fifo;
    // if (!(fifo = open(FIFO, O_RDWR)))
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
    // memset(buffer, 0x0, BUFSIZ);//?
    read(fifo, buffer, sizeof(buffer));
    cout << "Parent recieved sums: " << buffer << "\n";

    close(fifo);

    stringstream messageStream(buffer);

    int i = 0;
    double teamsValues[2];
    while (messageStream.good() && i < 2)
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