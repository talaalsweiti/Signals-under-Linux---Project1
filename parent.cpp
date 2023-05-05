#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>

using namespace std;
unsigned children[5];


void createProcesses(char*,int);
void generateRange();
void childDeadSignalCatcher(int);
void cleanup();

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
        for (int i = 0; i < strlen(argv[1]); i++)
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
    for (int i = 0; i < 4; i++)
    {
        createProcesses("./child",i);
    }
    
    createProcesses("./coprocessor",4);

    sigset(SIGCHLD, childDeadSignalCatcher);

    srand((unsigned) getpid());


    for(int i=0; i<rounds; i++){
        generateRange();
        
        for(int i=0; i<4; i++){
            kill(children[i], SIGUSR1);
        }
        
        sleep(10);
    }
    
    cleanup();
 
}

void createProcesses(char* file, int i)
{
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        perror("Fork");
        exit(4);

    case 0: /* In the child */
        execlp(file, file, (char *)NULL);
        perror("exec failure ");
        exit(-2);
        break;

    default: /* In the parent */
        children[i] = pid;
    }
}


void generateRange(){
    ofstream rangeFile;
    rangeFile.open("range.txt");

    int minValue = rand();
    int maxValue = rand();
    
    if(minValue > maxValue){
        swap(minValue, maxValue);
    }

    rangeFile << minValue << "," << maxValue;

    rangeFile.close();
}


void childDeadSignalCatcher(int theSig){
    cout << "A child failed\n";
    cleanup();
    exit(theSig);
    
}


void cleanup(){
    for(auto child: children){
        kill(child, SIGKILL);
    }
    exit(0);
}

/*
double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
*/