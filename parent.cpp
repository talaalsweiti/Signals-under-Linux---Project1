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

#define FIFO "/tmp/FIFO"
#define B_SIZ (PIPE_BUF / 2)
using namespace std;
unsigned children[5];

void createProcesses(int);
void generateRange();
string manageChildrenValues();
void childDeadSignalCatcher(int);
void childValueSignalCatcher(int);
void cleanup();
void sendValuesToCoprocessor(string);
void createCoProcessor();

unsigned sigCount = 0;

int f_des[2];

static char message[BUFSIZ];

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

    for (int i = 0; i < 4; i++)
    {
        createProcesses(i);
    }
    sleep(5);
    createCoProcessor();

    srand((unsigned)getpid());

    sleep(2);
    for (unsigned int i = 0; i < rounds; i++)
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

        sleep(1);
        close(f_des[0]);

        if (write(f_des[1], values.c_str(), strlen(values.c_str())) == -1)
        {
            perror("Write");
            exit(5);
        }

        sleep(1);

        if (read(f_des[0], message, BUFSIZ) == -1)
        {
            perror("Read");
            exit(4);
        }
        printf("Message received by parent: [%s]\n", message);
        fflush(stdout);
        close(f_des[1]);
    }

    cleanup();
}

void createProcesses(int i)
{
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        perror("Fork");
        exit(4);

    case 0: /* In the child */
        execlp("./child", "./child", (char *)NULL);
        perror("exec failure ");
        exit(-2);
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
    cout << "A child finished writing\n";
    fflush(stdout);
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
            exit(4);
        }

        string line;
        getline(childFile, line);
        values += line;
        if (i < 3)
        {
            values += ",";
        }
    }
    cout << values << "\n";
    return values;
    // send values to coprocessor via pipe
}

void createCoProcessor()
{
    if (pipe(f_des) == -1)
    {
        perror("Pipe");
        exit(8);
    }
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        perror("Fork");
        exit(4);

    case 0: /* In the child */
        char arg1[16];
        char arg2[16];
        sprintf(arg1, "%d", f_des[0]);
        sprintf(arg2, "%d", f_des[1]);

        execlp("./coprocessor", "./coprocessor", arg1, arg2, (char *)NULL);
        perror("exec failure ");
        exit(-2);

    default:
        children[4] = pid;
    }
}
void sendValuesToCoprocessor(string values)
{
}