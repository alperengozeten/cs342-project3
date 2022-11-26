#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

    char* inputFileName1 = argv[1];
    char* inputFileName2 = argv[2];
    int frameNum = atoi(argv[3]);
    char* outFileName = argv[4];
    char* algorithm = argv[6];

    FILE* inFile1 = fopen(inputFileName1, "r");
    FILE* inFile2 = fopen(inputFileName2, "r");
    FILE* outFile = fopen(outFileName, "w");

}