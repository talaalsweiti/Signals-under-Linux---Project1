# Multi-Processing Competition

This project aims to create a multi-processing application in Linux using signals, pipes, and fifos. 
The competition involves a parent process forking five children processes, which collaborate to perform calculations. 
The application runs in rounds, and the parent declares the winner after each round.

## Getting Started

Follow the steps below to set up and run the competition:

### Prerequisites
Make sure you have the following installed on your Linux machine:

*   gcc compiler
*   OpenGL library

### Installation

1. Clone the repository:
``` 
git clone https://github.com/yourusername/Signals-under-Linux---Project1
```
2. Navigate to the project directory:
```     
cd Signals-under-Linux---Project1
```

3. Compile the code:
```
gcc -o parent parent.c -lm -lpthread -lGL -lglut
gcc -o child child.c -lm
```

### Usage
Run the compiled parent executable:
```
./parent [number_of_rounds]
```

If no argument is provided, the application assumes 5 rounds.

## Competition Flow
1. The parent process forks five children processes.
2. The parent writes two comma-separated integer values (min and max) to a file called range.txt.
3. The first four processes generate floating values in the specified range, note them in files named after their PIDs, and inform the parent.
4. The parent sends the four values through a pipe to the fifth child process (co-processor).
5. The co-processor calculates two sums and returns them to the parent.
6. The parent decides the winning team based on the sums.
7. Steps 2 to 6 are repeated as many times as instructed by the user.
