#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <limits.h>
#include <sstream>

#define FIFO "/tmp/FIFO"

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
        double values[4];
        double teamSum[2];

        if ((fifo = open(FIFO, O_RDONLY)) == -1)
        {
            perror(FIFO);
            exit(7);
        }

        // memset(buffer, 0x0, BUFSIZ); 
        read(fifo, buffer, sizeof(buffer));

        close(fifo);

        stringstream messageStream(buffer);

        int i = 0;
        while (messageStream.good() && i < 4)
        {
            string substr;
            getline(messageStream, substr, ',');
            values[i++] = stod(substr);
            // cout << values[i - 1] << "\n";
            // fflush(stdout);
        }

        for (i = 0; i < 2; i++)
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