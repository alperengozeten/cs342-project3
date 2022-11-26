#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include <string.h>

int main(int argc, char* argv[]) {

    const int twoTo22 = 4194304;
    const int twoTo12 = 4096; 
    char* inputFileName1 = argv[1];
    char* inputFileName2 = argv[2];
    int frameNum = atoi(argv[3]);
    char* outFileName = argv[4];
    int algorithm = atoi(argv[6]);

    FILE* inFile1 = fopen(inputFileName1, "r");
    FILE* inFile2 = fopen(inputFileName2, "r");
    FILE* outFile = fopen(outFileName, "w");

    struct frame frames[frameNum];
    int frameCount = 0;
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
        long int vpn = virtualAdr / twoTo12; 
        long int firstTableIndex = virtualAdr / twoTo22; // divide by 2^22
        long int secondTableIndex = ((virtualAdr - twoTo22 * firstTableIndex) / twoTo12);
        int pageOffset = virtualAdr % twoTo12;

        int found = 0;
        for ( int i = 0; i < lineCount; i++ ) {
            if ( virtualAddresses[i].start <= virtualAdr && virtualAddresses[i].end > virtualAdr ) {
                found = 1;
                break;
            }
        }

        if ( found == 1 ) { // a valid virtual adress
            if ( outTable->tables[firstTableIndex].entries[secondTableIndex].validBit == 0 ) {
                if ( frameCount < frameNum ) {
                    // create a new frame
                    struct frame newFrame;
                    newFrame.vpn = vpn;
                    newFrame.occupied = 1;
                    
                    outTable->tables[firstTableIndex].entries[secondTableIndex].frameNum = frameCount; // put the frame count
                    frames[frameCount] = newFrame;

                    // write to file
                    char hex[4];
                    sprintf(hex, "%x", frameCount);

                    int length = 5 - strlen(hex); // get the number of preceeding zeros
                    char zeros[length + 1];
                    for ( int i = 0; i < length + 1; i++ ) {
                        zeros[i] = '0';
                    }
                    zeros[length] = '\0';

                    fprintf(outFile, "0x%s%s%c%c%c x\n", zeros, hex, word[7], word[8], word[9]);

                    frameCount++;
                }
                else { // apply algorithms to frame

                    if ( algorithm == 1 ) {
                        // evict with FIFO
                        long int evictedVPN = frames[0].vpn;
                        long int evictedFirstIndex = evictedVPN / 1024;
                        long int evictedSecondIndex = evictedVPN % 1024;

                        for ( int i = 0; i < frameCount - 1; i++ ) {
                            frames[i] = frames[i + 1];
                        }

                        frames[frameCount - 1].vpn = vpn;
                        frames[frameCount - 1].occupied = 1;

                        outTable->tables[firstTableIndex].entries[secondTableIndex].frameNum = frameCount - 1; // set the frame count

                        // set the valid bit of the evicted to 0
                        outTable->tables[evictedFirstIndex].entries[evictedSecondIndex].validBit = 0;
                        outTable->tables[evictedFirstIndex].entries[evictedSecondIndex].frameNum = -1;

                    }
                    else {
                        // evict with LRU
                    }
                }
            }
            else { // page exists in the physical memory
                // make LRU updates here
            }

            outTable->tables[firstTableIndex].entries[secondTableIndex].validBit = 1; // set the valid bit to 1 after finding a frame
        }
        else { // invalid virtual address, write it with e
            fprintf(outFile, "%s e\n", word);
        }
    }

    printf("%d\n", outTable->tables[64].entries[768].validBit);
    printf("%d\n", outTable->tables[64].entries[767].validBit);
    printf("frameCount: %d\n", frameCount);
    printf("%ld\n", frames[2].vpn);

    // some testing
    printf("%ld\n", virtualAddresses[2].end);
    printf("%d\n", outTable->tables[511].entries[2].validBit);

    fclose(inFile1);
    fclose(inFile2);
    fclose(outFile);
    free(outTable);
}