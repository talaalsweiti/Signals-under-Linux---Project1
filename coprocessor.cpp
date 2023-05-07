#include <local.h>

int fifo;

int main(int argc, char *argv[]) {
    char *arg1 = argv[1];

    printf("coprocessor\n");
    while (1)
    {
        if ((fifo = open(FIFO, O_RDWR)) == -1)
        {
            perror(FIFO);
            exit(7);
        }

    }
    return 0;
}