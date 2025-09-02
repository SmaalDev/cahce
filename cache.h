#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>

typedef enum {
    MAP_DIRECT = 1,
    MAP_FULLY_ASSOC = 2,
    MAP_SET_ASSOC = 3
} MappingPolicy;

typedef enum {
    REP_FIFO = 1,
    REP_LRU  = 2,
    REP_RANDOM = 3
} ReplacementPolicy;

typedef struct {
    int tag;
    bool valid;
    unsigned long lastUsed;
    unsigned long fifoSeq;
} CacheLine;

typedef struct {
    CacheLine* lines;
} CacheSet;

typedef struct {
    int numLines;
    int setSize;
    int numSets;
    MappingPolicy mapping;
    ReplacementPolicy replacement;
    CacheSet* sets;
    unsigned long clock;
    int hits;
    int misses;
} Cache;

Cache* createCache(int numLines, MappingPolicy mapping, ReplacementPolicy replacement);
void freeCache(Cache* c);
void printStats(const Cache* c);

#endif
