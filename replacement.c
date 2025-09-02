#include <stdlib.h>
#include "replacement.h"

static int findEmpty(const CacheSet* set, int ways) {
    for(int i=0;i<ways;i++) if(!set->lines[i].valid) return i;
    return -1;
}

static int fifo(const CacheSet* set,int ways) {
    unsigned long best=-1; int victim=0;
    for(int i=0;i<ways;i++){ if(!set->lines[i].valid) return i;
        if(set->lines[i].fifoSeq<best){best=set->lines[i].fifoSeq; victim=i;} }
    return victim;
}

static int lru(const CacheSet* set,int ways) {
    unsigned long best=-1; int victim=0;
    for(int i=0;i<ways;i++){ if(!set->lines[i].valid) return i;
        if(set->lines[i].lastUsed<best){best=set->lines[i].lastUsed; victim=i;} }
    return victim;
}

static int rnd(const CacheSet* set,int ways) {
    int empty=findEmpty(set,ways); if(empty>=0) return empty;
    return rand()%ways;
}

int chooseVictim(const Cache* c,const CacheSet* set) {
    switch(c->replacement) {
        case REP_FIFO: return fifo(set,c->setSize);
        case REP_LRU: return lru(set,c->setSize);
        case REP_RANDOM: return rnd(set,c->setSize);
        default: return 0;
    }
}
