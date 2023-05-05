#include <stdio.h>
#include <stdlib.h>
#include <fstream>

int main(){
    for(int i=0 ;i<3; i++){
        generateRange();

        sleep(20);
    }
    return 0;
}

void generateRange(){
    ofstream rangeFile;
    rangeFile.open("range.txt");

    srand((unsigned) getpid());

    int minValue = rand();
    int maxValue = rand();
    
    if(minValue > maxValue){
        swap(minValue, maxValue);
    }

    rangeFile << minValue << "," << maxValue;
}
