#include <stdlib.h> 

typedef struct virtualMem {
    long int start;
    long int end;
} virtualmem;


typedef struct frame {
    long int vpn;
    int order; // starts from 1, increases with the time
    int occupied; // 0 is unoccupied, 1 is occupied
} frame;

typedef struct tableEntry {
    long int frameNum;
    int validBit; // 0 is invalid, 1 is valid
} tableEntry;

typedef struct innerTable {
    struct tableEntry entries[1024];
} innerTable;

typedef struct outerTable {
    struct innerTable tables[1024];
} outerTable;