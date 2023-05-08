#include "local.h"

using namespace std;

void beginSignalCatcher(int);

// this the the first comment
int main()
{
    cout << "I am Child\n";
    srand((unsigned)getpid());

    if (sigset(SIGUSR1, beginSignalCatcher) == SIG_ERR)
    {
        perror("Sigset can not set SIGUSR1");
        exit(SIGUSR1);
    }
    while (1)
    {
        pause();
    }

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
}
