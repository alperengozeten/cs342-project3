#include <stdlib.h> 

typedef struct virtualMem {
    long int start;
    long int end;
} virtualmem;


typedef struct frame {
    long int vpn;
    bool occupied;
};

typedef struct tableEntry {
    long int frameNum;
    bool validBit;
};

typedef struct innerTable {
    struct tableEntry entries[1024];
};

typedef struct outerTable {
    struct innerTable tables[1024];
};