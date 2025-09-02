#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mapping.h"
#include "replacement.h"

static int RAM[] = {1,5,10,15,20, 3, 6, 9, 12, 18};
static const int TAM_RAM = sizeof(RAM)/sizeof(int);

static inline int computeSetIndex(const Cache* c, int tag) {
    return tag % c->numSets;
}

static int findLine(const CacheSet* set, int ways, int tag) {
    for (int i=0;i<ways;i++) if (set->lines[i].valid && set->lines[i].tag==tag) return i;
    return -1;
}

void accessAddress(Cache* c, int address) {
    int tag = address;
    int setIdx = computeSetIndex(c, tag);
    CacheSet* set = &c->sets[setIdx];
    c->clock++;

    int lineIdx = findLine(set, c->setSize, tag);
    if (lineIdx>=0) {
        c->hits++;
        set->lines[lineIdx].lastUsed = c->clock;
        return;
    }
    c->misses++;

    if (c->mapping==MAP_DIRECT) {
        CacheLine* line=&set->lines[0];
        line->tag=tag; line->valid=true; line->lastUsed=c->clock; line->fifoSeq=c->clock;
        return;
    }

    int victim=chooseVictim(c,set);
    set->lines[victim].tag=tag; set->lines[victim].valid=true;
    set->lines[victim].lastUsed=c->clock; set->lines[victim].fifoSeq=c->clock;
}

void simulate(Cache* c, const int* trace, int nTrace) {
    for(int i=0;i<nTrace;i++) {
        int addr=trace[i];
        int valid=0;
        for(int k=0;k<TAM_RAM;k++) if(RAM[k]==addr) { valid=1; break; }
        if(valid) accessAddress(c,addr);
    }
}
