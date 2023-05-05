#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fstream>

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
    // read range
    // ifstream rangeFile ("range.txt");
    // if(rangeFile.good()){
        
    // }
    // else{
    //     exit(2);
    // }
    // give a double
    // write double on file
}
