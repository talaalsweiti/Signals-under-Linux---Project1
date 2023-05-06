#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

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

    string line;

    getline(rangeFile, line);

    vector<string> rangeValues;
    stringstream sline(line);

    while(sline.good()){
        string substr;
        getline(sline, substr, ',');
        rangeValues.push_back(substr);
    }


    cout<< getpid() << " min :" << rangeValues[0]<<"\n";

    cout<< getpid() << " max :" << rangeValues[1]<<"\n";

    // int minValue = atoi (sMi)
    // double range = (atoi(max) - atoi(min)); 
    // double div = RAND_MAX / range;
    // double value =  min + (rand() / div);
    
    // // give a double
    // // write double on file
}
