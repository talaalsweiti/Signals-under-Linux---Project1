#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

void beginSignalCatcher(int);

int main()
{
    cout << "I am Child\n";

    if (sigset(SIGUSR1, beginSignalCatcher) == SIG_ERR)
    {
        perror("Sigset can not set SIGUSR1");
        exit(SIGUSR1);
    }
    while (1)
        ;
    return 0;
}

void beginSignalCatcher(int theSig) // generateValue
{
    cout << "tala\n";
    fflush(stdout);
    // read range
    ifstream rangeFile("/tmp/range.txt");
    if (!rangeFile.good())
    {
        perror("Open range.txt");
        exit(2);
    }

    string line;

    getline(rangeFile, line);

    vector<string> rangeValues;
    stringstream sline(line);

    while (sline.good())
    {
        string substr;
        getline(sline, substr, ',');
        rangeValues.push_back(substr);
    }
    // check for errors (empty line ...)

    srand((unsigned)getpid());

    int minValue = stoi(rangeValues[0]);
    int maxValue = stoi(rangeValues[1]);
    double range = (maxValue - minValue);
    double div = RAND_MAX / range;
    double value = minValue + (rand() / div);

    ofstream generatedNumFile;
    string fileName = "/tmp/" + to_string(getpid()) + ".txt";
    generatedNumFile.open(fileName);

    generatedNumFile << value;

    generatedNumFile.close();

    cout << "shahd\n";
    fflush(stdout);

    kill(getppid(), SIGUSR1);

    // cout << to_string(getpid()) << "\n";

    // // give a double
    // // write double on file
}
