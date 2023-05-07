#include "local.h"

using namespace std;
int fifo;
char buffer[BUFSIZ];

int main()
{

    // if ((fifo = open(FIFO, O_RDWR)) == -1)
    // {
    //     perror(FIFO);
    //     exit(7);
    // }
    printf("coprocessor\n");
    while (1)
    {
        double values[NUM_OF_CHILDREN-1];
        double teamSum[NUM_OF_TEAMS];

        if ((fifo = open(FIFO, O_RDONLY)) == -1)
        {
            perror(FIFO);
            exit(7);
        }

        // memset(buffer, 0x0, BUFSIZ); 
        read(fifo, buffer, sizeof(buffer));

        close(fifo);

        stringstream messageStream(buffer);

        unsigned int i = 0;
        while (messageStream.good() && i < NUM_OF_CHILDREN-1)
        {
            string substr;
            getline(messageStream, substr, ',');
            values[i++] = stod(substr);
        }

        for (i = 0; i < NUM_OF_TEAMS; i++)
        {
            teamSum[i] = values[i << 1] + values[(i << 1) + 1];
        }
        string teamsResult = to_string(teamSum[0]) + "," + to_string(teamSum[1]);

        cout << "Coprocessor:: team1 sum: " << teamSum[0] << "\n";
        cout << "Coprocessor:: team2 sum: " << teamSum[1] << "\n";
        fflush(stdout);


        if ((fifo = open(FIFO, O_WRONLY)) == -1)
        {
            perror(FIFO);
            exit(7);
        }

        write(fifo, teamsResult.c_str(), teamsResult.length() + 1);

        close(fifo);
        // while (1)
        //     ;
    }
    close(fifo);
    return 0;
}