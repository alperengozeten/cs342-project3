#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include <string.h>

long generateRandomAddress(long addressHigh) {
    return rand() / (RAND_MAX / addressHigh + 1);
}

int main(int argc, char* argv[]) {

    const int twoTo22 = 4194304;
    const int twoTo12 = 4096; 
    char* inputFileName1; 
    char* inputFileName2;
    int frameNum;
    int algorithm;
    char* outFileName;
    long vmsize;
    int addressCount;
    FILE* inFile1;
    FILE* inFile2;

    if ( argc == 7 ) {
        inputFileName1 = argv[1];
        inputFileName2 = argv[2];
        frameNum = atoi(argv[3]);
        outFileName = argv[4];
        algorithm = atoi(argv[6]);

        inFile1 = fopen(inputFileName1, "r");
        inFile2 = fopen(inputFileName2, "r");
    }
    else {
        frameNum = atoi(argv[1]);
        outFileName = argv[2];
        algorithm = atoi(argv[4]);
        vmsize = atoi(argv[6]);
        addressCount = atoi(argv[8]);
    }

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
    if ( argc == 7 ) {
        word[64];
        char c;
        addressCount = 1;
        //fscanf(inFile1, "%s", word) == 1;

        for ( c = getc(inFile1); c != EOF; c = getc(inFile1) ) {
            if ( c == '\n' ) {
                addressCount++;
            }
        }
    }

    rewind(inFile1); // set the pointer to the first line

    struct virtualMem virtualAddresses[addressCount]; // create an array of struct
    int index = 0;

    while ( fscanf(inFile1, "%s", word) == 1 ) {
        long int startAdr = strtol(word, NULL, 0);
        fscanf(inFile1, "%s", word); 
        long int endAdr = strtol(word, NULL, 0);

        virtualAddresses[index].start = startAdr;
        virtualAddresses[index].end = endAdr;
        index++;
    }

    int timeCounter = 1;
    int done = 0;
    // Scan the file containing virtual addresses to transform
    if ( argc == 7 ) {
        done = fscanf(inFile2, "%s", word);
    }
    while ( done == 1 ) {
        long int virtualAdr = strtol(word, NULL, 0); // convert to decimal
        long int vpn = virtualAdr / twoTo12; 
        long int firstTableIndex = virtualAdr / twoTo22; // divide by 2^22
        long int secondTableIndex = ((virtualAdr - twoTo22 * firstTableIndex) / twoTo12);
        int pageOffset = virtualAdr % twoTo12;

        int found = 0;
        for ( int i = 0; i < addressCount; i++ ) {
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
                    if ( algorithm == 2 ) {
                        newFrame.order = 1;
                    }
                    else {
                        newFrame.order = timeCounter;
                        timeCounter++;
                    }
                    
                    outTable->tables[firstTableIndex].entries[secondTableIndex].frameNum = frameCount; // put the frame number
                    frames[frameCount] = newFrame;

                    if ( algorithm == 2 ) {
                        // increase the order of the others
                        for ( int i = 0; i < frameCount; i++ ) {
                            frames[i].order++;
                        }
                    }

                    // convert to hex
                    char hex[4];
                    sprintf(hex, "%x", frameCount);

                    int length = 5 - strlen(hex); // get the number of preceeding zeros
                    char zeros[length + 1];
                    for ( int i = 0; i < length + 1; i++ ) {
                        zeros[i] = '0';
                    }
                    zeros[length] = '\0';

                    // write to file
                    fprintf(outFile, "0x%s%s%c%c%c x\n", zeros, hex, word[7], word[8], word[9]);

                    frameCount++;
                }
                else { // apply algorithms to frame
                    int evictedIndex;

                    if ( algorithm == 2 ) {
                        // evict with FIFO

                        int maxIndex = -1;
                        int maxOrder = -1;
                        
                        // find the oldest one
                        for ( int i = 0; i < frameCount; i++ ) {
                            if ( frames[i].order > maxOrder ) {
                                maxIndex = i;
                                maxOrder = frames[i].order;
                            }
                        }

                        // get the evicted vpn
                        evictedIndex = maxIndex;

                    }
                    else {
                        // evict with LRU
                        int minIndex = -1;
                        int minOrder = __INT_MAX__;
                        
                        // find the oldest one
                        for ( int i = 0; i < frameCount; i++ ) {
                            if ( frames[i].order < minOrder ) {
                                minIndex = i;
                                minOrder = frames[i].order;
                            }
                        }

                        evictedIndex = minIndex;
                    }

                    printf("evicted frame: %d\n", evictedIndex);
                    long int evictedVPN = frames[evictedIndex].vpn;
                    long int evictedFirstIndex = evictedVPN / 1024;
                    long int evictedSecondIndex = evictedVPN % 1024;

                    frames[evictedIndex].vpn = vpn;
                    frames[evictedIndex].occupied = 1;

                    if ( algorithm == 2 ) {
                        frames[evictedIndex].order = 1;

                        for ( int i = 0; i < frameCount; i++ ) {
                            if ( i != evictedIndex ) {
                                frames[i].order++;
                            }
                        }
                    }
                    else {
                        frames[evictedIndex].order = timeCounter;
                        timeCounter++;
                    }

                    outTable->tables[firstTableIndex].entries[secondTableIndex].frameNum = evictedIndex; // set the frame number

                    // set the valid bit of the evicted to 0
                    outTable->tables[evictedFirstIndex].entries[evictedSecondIndex].validBit = 0;
                    outTable->tables[evictedFirstIndex].entries[evictedSecondIndex].frameNum = -1;

                    // output to the file
                    int frameNumber = evictedIndex;
                    // convert to hex
                    char hex[4];
                    sprintf(hex, "%x", frameNumber);

                    int length = 5 - strlen(hex); // get the number of preceeding zeros
                    char zeros[length + 1];
                    for ( int i = 0; i < length + 1; i++ ) {
                        zeros[i] = '0';
                    }
                    zeros[length] = '\0';

                    // write to file
                    fprintf(outFile, "0x%s%s%c%c%c x\n", zeros, hex, word[7], word[8], word[9]);
                }
            }
            else { // page exists in the physical memory
                // make LRU updates here
                
                int frameNumber = outTable->tables[firstTableIndex].entries[secondTableIndex].frameNum;

                if ( algorithm == 1 ) {
                    frames[frameNumber].order = timeCounter;
                    timeCounter++;
                }
                printf("exists, frame num: %d\n", frameNumber);

                // convert to hex
                char hex[4];
                sprintf(hex, "%x", frameNumber);

                int length = 5 - strlen(hex); // get the number of preceeding zeros
                char zeros[length + 1];
                for ( int i = 0; i < length + 1; i++ ) {
                    zeros[i] = '0';
                }
                zeros[length] = '\0';

                // write to file
                fprintf(outFile, "0x%s%s%c%c%c \n", zeros, hex, word[7], word[8], word[9]);
            }

            outTable->tables[firstTableIndex].entries[secondTableIndex].validBit = 1; // set the valid bit to 1 after finding a frame
        }
        else { // invalid virtual address, write it with e
            fprintf(outFile, "%s e\n", word);
        }

        if ( argc == 7 ) {
            done = fscanf(inFile2, "%s", word);
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