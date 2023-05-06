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
void manageChildrenValues();
void childDeadSignalCatcher(int);
void childValueSignalCatcher(int);
void cleanup();

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
    for (int i = 0; i < 4; i++)
    {
        createProcesses("./child",i);
    }
    
    createProcesses("./coprocessor",4);

    sigset(SIGCHLD, childDeadSignalCatcher);
    sigset(SIGUSR1, childValueSignalCatcher);

    srand((unsigned) getpid());

    sleep(2);
    for(unsigned int i=0; i<rounds; i++){
        sigCount = 0;
        generateRange();
        
        for(int i=0; i<4; i++){
            kill(children[i], SIGUSR1);
        }

        while(sigCount<4); // pause() function caused the process not to recieve all signals sent by children

        cout<< "children finished writing\n";
        fflush(stdout);

        manageChildrenValues();
           
        

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

void childValueSignalCatcher(int theSig){
    cout<< "A child finished writing\n";
    fflush(stdout);
    sigCount++;
}


void cleanup(){
    sigset(SIGCHLD, SIG_DFL);

    for(auto child: children){
        kill(child, SIGKILL);
    }

    for(int i=0; i<4; i++){
        string fileName = to_string(children[i]) + ".txt";
        int result = unlink(fileName.c_str());
        // if(!result){
        //     // if was not successfull do sth
        //     // if the program terminated before creating the file?
        // }
    }
    exit(0);
}

void manageChildrenValues(){

    string values;
    for(int i=0; i<4; i++){
        string fileName = to_string(children[i]) + ".txt";
        ifstream childFile(fileName);
        if (!childFile.good())
        {
            perror("Open child file");
            exit(4);
        }

        string line;
        getline(childFile, line);
        values+=line;
        if(i<3){
            values+=",";
        }
    }
    cout<<values<<"\n";

    // send values to coprocessor via pipe
}