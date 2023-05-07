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
int fifo;

int f_des[2];
static char message[BUFSIZ];

using namespace std;

int main(int argc, char *argv[])
{
    sleep(3);
    f_des[0] = atoi(argv[1]);
    f_des[1] = atoi(argv[2]);

    printf("Im coprocessor\n");
    fflush(stdout);

    // close(f_des[1]);
    if (read(f_des[0], message, BUFSIZ) == -1)
    {
        perror("Read");
        exit(4);
    }
    printf("Message received by child: [%s]\n", message);
    fflush(stdout);

    sleep(1);

    if (write(f_des[1], message, strlen(message) == -1))
    {
        perror("Write");
        exit(5);
    }
    // close(f_des[0]);

    while (1)
        ;

    return 0;
}