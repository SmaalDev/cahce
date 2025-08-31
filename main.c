#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ======================= Structs =========================
typedef struct {
    int valid;      // 0 = inválido, 1 = válido
    int tag;        // Tag do bloco
    int counter;    // Usado em LRU ou FIFO
} CacheLine;

typedef struct {
    int numLines;       // Total de linhas
    int setSize;        // Linhas por conjunto
    CacheLine *lines;   // Array de linhas
    int hits;
    int misses;
} Cache;

// ======================= Prototypes =========================
Cache *createCache(int numLines, int setSize);
void freeCache(Cache *cache);
void printStats(Cache *cache);

void startSimulation(int mappingPolicy, int replacementPolicy);

void directMapping(Cache *cache);
void associativeMapping(Cache *cache, int replacementPolicy);
void setAssociativeMapping(Cache *cache, int replacementPolicy);

void replacementFIFO(Cache *cache, int setIndex);
void replacementLRU(Cache *cache, int setIndex);
void replacementRandom(Cache *cache, int setIndex);

int menuMappingPolicy();
int menuReplacementPolicy();

// =============================================================

int main() {
    int mappingPolicy, replacementPolicy = 0;

    printf("=== Cache Simulator ===\n");

    mappingPolicy = menuMappingPolicy();

    if (mappingPolicy == 2 || mappingPolicy == 3) {
        replacementPolicy = menuReplacementPolicy();
    }

    startSimulation(mappingPolicy, replacementPolicy);

    return 0;
}

// =============================================================
// Funções auxiliares comuns

Cache *createCache(int numLines, int setSize) {
    Cache *cache = (Cache*) malloc(sizeof(Cache));
    cache->numLines = numLines;
    cache->setSize = setSize;
    cache->hits = 0;
    cache->misses = 0;

    cache->lines = (CacheLine*) malloc(sizeof(CacheLine) * numLines);
    for (int i = 0; i < numLines; i++) {
        cache->lines[i].valid = 0;
        cache->lines[i].tag = -1;
        cache->lines[i].counter = 0;
    }

    return cache;
}

void freeCache(Cache *cache) {
    free(cache->lines);
    free(cache);
}

void printStats(Cache *cache) {
    printf("\n=== Simulation Results ===\n");
    printf("Hits: %d\n", cache->hits);
    printf("Misses: %d\n", cache->misses);
    float hitRate = (float) cache->hits / (cache->hits + cache->misses);
    printf("Hit Rate: %.2f%%\n", hitRate * 100);
}

// =============================================================
// Menu

int menuMappingPolicy() {
    int option;
    printf("\nChoose Mapping Policy:\n");
    printf("1 - Direct Mapping\n");
    printf("2 - Fully Associative Mapping\n");
    printf("3 - Set-Associative Mapping\n");
    printf("Option: ");
    scanf("%d", &option);
    return option;
}

int menuReplacementPolicy() {
    int option;
    printf("\nChoose Replacement Policy:\n");
    printf("1 - FIFO\n");
    printf("2 - LRU\n");
    printf("3 - Random\n");
    printf("Option: ");
    scanf("%d", &option);
    return option;
}

// =============================================================
// Controle da simulação

void startSimulation(int mappingPolicy, int replacementPolicy) {
    printf("\n--- Starting Simulation ---\n");

    // Exemplo: vamos criar uma cache com 8 linhas, conjuntos de 2
    Cache *cache = createCache(8, (mappingPolicy == 3) ? 2 : 1);

    if (mappingPolicy == 1) {
        directMapping(cache);
        printf(">> Direct mapping does not use replacement policies.\n");
    }
    else {
        if (mappingPolicy == 2) {
            associativeMapping(cache, replacementPolicy);
        }
        else {
            if (mappingPolicy == 3) {
                setAssociativeMapping(cache, replacementPolicy);
            }
            else {
                printf("Invalid mapping policy!\n");
                freeCache(cache);
                return;
            }
        }
    }

    printStats(cache);
    freeCache(cache);
}

// =============================================================
// Funções de mapeamento

void directMapping(Cache *cache) {
    printf(">> Using Direct Mapping...\n");
    // TODO: Implementar a lógica de mapeamento direto
}

void associativeMapping(Cache *cache, int replacementPolicy) {
    printf(">> Using Fully Associative Mapping...\n");

    if (replacementPolicy == 1) {
        replacementFIFO(cache, 0);
    }
    else {
        if (replacementPolicy == 2) {
            replacementLRU(cache, 0);
        }
        else {
            if (replacementPolicy == 3) {
                replacementRandom(cache, 0);
            }
        }
    }

    // TODO: Implementar a lógica associativa
}

void setAssociativeMapping(Cache *cache, int replacementPolicy) {
    printf(">> Using Set-Associative Mapping...\n");

    if (replacementPolicy == 1) {
        replacementFIFO(cache, 0);
    }
    else {
        if (replacementPolicy == 2) {
            replacementLRU(cache, 0);
        }
        else {
            if (replacementPolicy == 3) {
                replacementRandom(cache, 0);
            }
        }
    }

    // TODO: Implementar a lógica de conjunto-associativo
}

// =============================================================
// Funções de substituição

void replacementFIFO(Cache *cache, int setIndex) {
    printf(">> Replacement Policy: FIFO\n");
    // TODO: Implementar substituição FIFO dentro do conjunto
}

void replacementLRU(Cache *cache, int setIndex) {
    printf(">> Replacement Policy: LRU\n");
    // TODO: Implementar substituição LRU dentro do conjunto
}

void replacementRandom(Cache *cache, int setIndex) {
    printf(">> Replacement Policy: Random\n");
    srand(time(NULL));
    // TODO: Implementar substituição aleatória dentro do conjunto
}
