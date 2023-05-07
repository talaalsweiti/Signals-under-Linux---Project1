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

#define FIFO "/tmp/FIFO"
#define B_SIZ (PIPE_BUF / 2)

// struct message
// {
//     char fifo_name[B_SIZ];
//     char cmd_line[B_SIZ];
// };