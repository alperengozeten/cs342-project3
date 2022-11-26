#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

int main(int argc, char* argv[]) {

    char* inputFileName1 = argv[1];
    char* inputFileName2 = argv[2];
    int frameNum = atoi(argv[3]);
    char* outFileName = argv[4];
    char* algorithm = argv[6];

    FILE* inFile1 = fopen(inputFileName1, "r");
    FILE* inFile2 = fopen(inputFileName2, "r");
    FILE* outFile = fopen(outFileName, "w");

    char word[64];
    char c;
    int lineCount = 1;
    //fscanf(inFile1, "%s", word) == 1;

    for ( c = getc(inFile1); c != EOF; c = getc(inFile1) ) {
        if ( c == '\n' ) {
            lineCount++;
        }
    }

    rewind(inFile1); // set the pointer to the first line

    struct virtualMem virtualAddresses[lineCount]; // create an array of struct
    int index = 0;

    while ( fscanf(inFile1, "%s", word) == 1 ) {
        long int startAdr = strtol(word, NULL, 0);
        fscanf(inFile1, "%s", word); 
        long int endAdr = strtol(word, NULL, 0);

        virtualAddresses[index].start = startAdr;
        virtualAddresses[index].end = endAdr;
        index++;
    }

    fclose(inFile1);
    fclose(inFile2);
    fclose(outFile);
}