#include "local.h"

using namespace std;

void beginSignalCatcher(int);

int main()
{
    cout << "I am Child with PID " << getpid() << "\n";
    
    srand((unsigned)getpid()); /* Set child PID as unique seed for rand */

    /* set up signal handler when SIGUSR1 is called by the parent */
    if (sigset(SIGUSR1, beginSignalCatcher) == SIG_ERR)
    {
        perror("Sigset can not set SIGUSR1");
        exit(SIGUSR1);
    }

    /* suspend the child process until a signal is called */
    while (1)
    {
        pause();
    }

    return 0;
}

/*
    This function is the SIGUSR1 Signal Handler.
    It reads the range values and sends back a random generated value between MIN and MAX
*/
void beginSignalCatcher(int theSig)
{

    /* read MIN and MAX from range.txt */
    ifstream rangeFile("/tmp/range.txt");
    if (!rangeFile.good())
    {
        perror("Open range.txt");
        exit(2);
    }
    string line;
    getline(rangeFile, line);

    /* split string <MIN>,<MAX> into the array rangeValues */
    vector<string> rangeValues(2);
    unsigned int i = 0;
    stringstream sline(line);

    while (sline.good())
    {
        string substr;
        getline(sline, substr, ',');
        rangeValues[i++] = substr;
    }

    /* convert MIN and MAX from string to int */
    int minValue = stoi(rangeValues[0]);
    int maxValue = stoi(rangeValues[1]);

    /* generate random value in range using rand() function */
    double range = (maxValue - minValue); 
    double div = RAND_MAX / range;
    double value = minValue + (rand() / div);

    /* write the generated value to <PID>.txt file */
    ofstream generatedNumFile;
    string fileName = "/tmp/" + to_string(getpid()) + ".txt";
    generatedNumFile.open(fileName);

    generatedNumFile << value;

    generatedNumFile.close();

    /* send signal to parent to inform of finishing writing */
    kill(getppid(), SIGUSR1);
}
