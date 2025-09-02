#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

Cache* createCache(int numLines, MappingPolicy mapping, ReplacementPolicy replacement) {
    Cache* c = (Cache*)malloc(sizeof(Cache));
    if (!c) { fprintf(stderr, "Erro ao alocar Cache\n"); exit(EXIT_FAILURE); }

    c->numLines = numLines;
    c->mapping = mapping;
    c->replacement = replacement;
    c->clock = 0;
    c->hits = 0;
    c->misses = 0;

    if (mapping == MAP_DIRECT) c->setSize = 1;
    else if (mapping == MAP_FULLY_ASSOC) c->setSize = numLines;
    else c->setSize = 2;

    c->numSets = numLines / c->setSize;
    c->sets = (CacheSet*)calloc(c->numSets, sizeof(CacheSet));
    for (int s = 0; s < c->numSets; ++s) {
        c->sets[s].lines = (CacheLine*)calloc(c->setSize, sizeof(CacheLine));
    }
    return c;
}

void freeCache(Cache* c) {
    if (!c) return;
    for (int s = 0; s < c->numSets; ++s) free(c->sets[s].lines);
    free(c->sets);
    free(c);
}

void printStats(const Cache* c) {
    printf("\n=== Resultados da Simulacao ===\n");
    printf("Linhas: %d | Conjuntos: %d | Vias: %d\n", c->numLines, c->numSets, c->setSize);
    printf("Hits: %d | Misses: %d\n", c->hits, c->misses);
    int total = c->hits + c->misses;
    double rate = (total>0)?(100.0*c->hits/total):0.0;
    printf("Taxa de acerto: %.2f%%\n", rate);
}
