#include "local.h"

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

    // read range
    ifstream rangeFile("/tmp/range.txt");
    if (!rangeFile.good())
    {
        perror("Open range.txt");
        exit(2);
    }

    string line;

    getline(rangeFile, line);

    vector<string> rangeValues(2);
    unsigned int i = 0;
    stringstream sline(line);

    while (sline.good())
    {
        string substr;
        getline(sline, substr, ',');
        rangeValues[i++] = substr;
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
 

    kill(getppid(), SIGUSR1);
  

    // // give a double
    // // write double on file
}
