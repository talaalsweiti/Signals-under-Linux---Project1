/*
    The coprocessor receives the generated value from the parent and calculates
    the sum for each team. Then it returns the sums to the parent to decide the round winner
*/

#include "local.h"

using namespace std;
int fifo;
char buffer[BUFSIZ];

int main()
{

    cout << "I am Coprocessor with PID " << getpid() << "\n";

    /**/
    while (1)
    {
        double values[NUM_OF_CHILDREN - 1]; /* all children processes excepts for coprocessor */
        double teamSum[NUM_OF_TEAMS]; 

        /* try to open the FIFO for reading */
        if ((fifo = open(FIFO, O_RDONLY)) == -1)
        {
            perror(FIFO);
            exit(7);
        }

        memset(buffer, 0x0, BUFSIZ); /* set size of buffer and initialize bytes to 0 */
        read(fifo, buffer, sizeof(buffer)); /* read FIFO and save to buffer (blocking operation) */

        cout << "MESSAGE RECIVED IN PROCESSOR: " << buffer << endl;

        close(fifo); /* close FIFO to open later for writing */

        /* split string <C1_VALUE>,<C2_VALUE>,<C3_VALUE>,<C4_VALUE> into the array values */        
        stringstream messageStream(buffer);

        unsigned int i = 0;
        while (messageStream.good() && i < NUM_OF_CHILDREN - 1)
        {
            string substr;
            getline(messageStream, substr, ',');
            values[i++] = stod(substr);
        }

        /* Summing every 2 values and save it in teamSum[i] (the sum for the team) */
        for (i = 0; i < NUM_OF_TEAMS; i++)
        {
            teamSum[i] = values[i << 1] + values[(i << 1) + 1];
        }

        cout << "Coprocessor:: team1 sum: " << teamSum[0] << endl;
        cout << "Coprocessor:: team2 sum: " << teamSum[1] << endl;

        string teamsResult = to_string(teamSum[0]) + "," + to_string(teamSum[1]); /* putting result in one string to send it in FIFO */
        
        /* Open FIFO for writing */
        if ((fifo = open(FIFO, O_WRONLY)) == -1)
        {
            perror(FIFO);
            exit(7);
        }

        write(fifo, teamsResult.c_str(), teamsResult.length() + 1); /* write teamResult string to FIFO to be read by the parent */
        
        cout << "MESSAGE WROTE BY PROCESSOR : " << teamsResult << endl;
        fflush(stdout);
        
        close(fifo); // close the FIFO
    }
    return 0;
}