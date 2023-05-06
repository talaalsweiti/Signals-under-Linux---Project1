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
    cout<<"I am Child\n";

    if (sigset(SIGUSR1, beginSignalCatcher) == SIG_ERR)
    {
        perror("Sigset can not set SIGUSR1");
        exit(SIGUSR1);
    }
    while(1);
    return 0;
}

void beginSignalCatcher(int theSig)
{
    cout<<"tala\n";
    fflush(stdout);
    // read range
    ifstream rangeFile ("range.txt");
    if(!rangeFile.good()){
        perror("Open range.txt");
        exit(2);
    }

    
    string sMinValue;
    if(!getline(rangeFile, sMinValue, ',')){
        perror("Reading range.txt");
        exit(3);
    }

    // cout<< getpid() << " min :" << minValue<<"\n";

    string sMaxValue;
    if(!getline(rangeFile, sMaxValue, ',')){
         perror("Reading range.txt")
        exit(3);
    }

    // cout<< getpid() << " max :" << maxValue<<"\n";

    int minValue = atoi (sMi)
    double range = (atoi(max) - atoi(min)); 
    double div = RAND_MAX / range;
    double value =  min + (rand() / div);
    
    // give a double
    // write double on file
}
