#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>

using namespace std;

void beginSignalCatcher(int);

int main()
{
    if (sigset(SIGUSR1, beginSignalCatcher) == SIG_ERR)
    {
        perror("Sigset can not set SIGINT");
        exit(SIGUSR1);
    }
    return 0;
}

void beginSignalCatcher(int theSig)
{
    cout<<"tala\n";
    fflush(stdout);
    // read range
    ifstream rangeFile ("range.txt");
    if(!rangeFile.good()){
        cout<<"here1\n";
        fflush(stdout);
        exit(2);
    }

    string line;
    if(!getline(rangeFile, line, ',')){
        cout<<"here2\n";
        fflush(stdout);
        exit(3);
    }

    cout<<line<<"\n";

    // stringstream sline (line);

    // while(sline.good()){
    //     string substr; 
    //     getline
    // }
    
    // give a double
    // write double on file
}
