#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cache.h"
#include "mapping.h"
#include "utils.h"

#define NUMBER_TRACE 20

int main(void) {
    srand((unsigned int)time(NULL));

    printf("=== Simulador de Cache ===\n");
    int mapping = menuMappingPolicy();
    int replacement = (mapping == MAP_FULLY_ASSOC || mapping == MAP_SET_ASSOC)
                        ? menuReplacementPolicy()
                        : REP_FIFO;

    Cache* cache = createCache(8, (MappingPolicy)mapping, (ReplacementPolicy)replacement);

    int trace[NUMBER_TRACE];
    for (int i = 0; i < NUMBER_TRACE; i++) {
        trace[i] = rand() % 20;
    }
    int nTrace = sizeof(trace)/sizeof(int);

    printf("\n--- Iniciando Simulacao ---\n");
    simulate(cache, trace, nTrace);
    printStats(cache);

    freeCache(cache);
    return 0;
}
