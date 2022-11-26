#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

int main(int argc, char* argv[]) {

    const int twoTo22 = 4194304;
    char* inputFileName1 = argv[1];
    char* inputFileName2 = argv[2];
    int frameNum = atoi(argv[3]);
    char* outFileName = argv[4];
    char* algorithm = argv[6];

    FILE* inFile1 = fopen(inputFileName1, "r");
    FILE* inFile2 = fopen(inputFileName2, "r");
    FILE* outFile = fopen(outFileName, "w");

    struct frame frames[frameNum];
    for ( int i = 0; i < frameNum; i++ ) {
        frames[i].occupied = 0; // all frames are empty initially
    }
    
    struct outerTable* outTable = malloc(sizeof(struct outerTable)); // allocate from the heap
    for ( int i = 0; i < 1024; i++ ) {
        for ( int j = 0; j < 1024; j++ ) {
            outTable->tables[i].entries[j].validBit = 0; // set all the valid bits to invalid initially
        }
    }
    

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

    // Scan the file containing virtual addresses to transform
    while ( fscanf(inFile2, "%s", word) == 1 ) {
        long int virtualAdr = strtol(word, NULL, 0); // convert to decimal
        long int firstTableIndex = virtualAdr / twoTo22; // divide by 
        printf("first index: %ld\n", firstTableIndex);
    }

    // some testing
    printf("%ld\n", virtualAddresses[2].end);
    printf("%d\n", outTable->tables[511].entries[2].validBit);

    fclose(inFile1);
    fclose(inFile2);
    fclose(outFile);
    free(outTable);
}