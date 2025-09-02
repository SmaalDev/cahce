#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// ======================= RAM de exemplo =========================
static int RAM[] = {1, 5, 10, 15, 20};
static const int TAM_RAM = sizeof(RAM) / sizeof(int);

// ======================= Políticas ==============================
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

// ======================= Estruturas =============================
typedef struct {
    int tag;                 // "endereço/bloco" (tag)
    bool valid;              // linha válida?
    unsigned long lastUsed;  // p/ LRU
    unsigned long fifoSeq;   // p/ FIFO (ordem de chegada)
} CacheLine;

typedef struct {
    CacheLine* lines;        // vetor de "ways"
} CacheSet;

typedef struct {
    int numLines;            // linhas totais
    int setSize;             // vias (ways) por conjunto
    int numSets;             // quantidade de conjuntos
    MappingPolicy mapping;
    ReplacementPolicy replacement;

    CacheSet* sets;          // vetor de conjuntos
    unsigned long clock;     // contador global (tempo lógico)
    int hits;
    int misses;
} Cache;

// ======================= Protótipos =============================
Cache* createCache(int numLines, MappingPolicy mapping, ReplacementPolicy replacement);
void   freeCache(Cache* c);
void   printStats(const Cache* c);
void   simulate(Cache* c, const int* trace, int nTrace);

// Acesso (resolve hit/miss + substituição)
void   accessAddress(Cache* c, int address);

// Helpers de substituição
int    chooseVictim(const Cache* c, const CacheSet* set);
int    chooseVictimFIFO(const CacheSet* set, int ways);
int    chooseVictimLRU(const CacheSet* set, int ways);
int    chooseVictimRandom(const CacheSet* set, int ways);
int    findLine(const CacheSet* set, int ways, int tag);
int    findEmpty(const CacheSet* set, int ways);

// Menus
int    menuMappingPolicy(void);
int    menuReplacementPolicy(void);

// ======================= Implementação ==========================
Cache* createCache(int numLines, MappingPolicy mapping, ReplacementPolicy replacement) {
    Cache* c = (Cache*)malloc(sizeof(Cache));
    if (!c) {
        fprintf(stderr, "Erro ao alocar Cache\n");
        exit(EXIT_FAILURE);
    }

    c->numLines = numLines;
    c->mapping = mapping;
    c->replacement = replacement;
    c->clock = 0;
    c->hits = 0;
    c->misses = 0;

    // Definição de vias por conjunto conforme mapeamento
    if (mapping == MAP_DIRECT) {
        c->setSize = 1;                 // direto: 1 via
    } else if (mapping == MAP_FULLY_ASSOC) {
        c->setSize = numLines;          // totalmente associativo: 1 conjunto, N vias
    } else { // MAP_SET_ASSOC
        c->setSize = 2;                 // conjunto-associativo com 2 vias (padrão)
    }

    // Quantidade de conjuntos
    c->numSets = numLines / c->setSize;
    if (c->numSets <= 0 || numLines % c->setSize != 0) {
        fprintf(stderr, "Configuração invalida: numLines=%d, setSize=%d\n", numLines, c->setSize);
        free(c);
        exit(EXIT_FAILURE);
    }

    c->sets = (CacheSet*)calloc(c->numSets, sizeof(CacheSet));
    if (!c->sets) {
        fprintf(stderr, "Erro ao alocar conjuntos\n");
        free(c);
        exit(EXIT_FAILURE);
    }

    for (int s = 0; s < c->numSets; ++s) {
        c->sets[s].lines = (CacheLine*)calloc(c->setSize, sizeof(CacheLine));
        if (!c->sets[s].lines) {
            fprintf(stderr, "Erro ao alocar linhas do conjunto %d\n", s);
            // Liberação parcial
            for (int k = 0; k < s; ++k) free(c->sets[k].lines);
            free(c->sets);
            free(c);
            exit(EXIT_FAILURE);
        }
        // Inicializa linhas
        for (int w = 0; w < c->setSize; ++w) {
            c->sets[s].lines[w].valid = false;
            c->sets[s].lines[w].tag = 0;
            c->sets[s].lines[w].lastUsed = 0;
            c->sets[s].lines[w].fifoSeq = 0;
        }
    }

    return c;
}

void freeCache(Cache* c) {
    if (!c) return;
    for (int s = 0; s < c->numSets; ++s) {
        free(c->sets[s].lines);
    }
    free(c->sets);
    free(c);
}

void printStats(const Cache* c) {
    printf("\n=== Resultados da Simulacao ===\n");
    printf("Linhas totais: %d | Conjuntos: %d | Vias por conjunto: %d\n",
           c->numLines, c->numSets, c->setSize);
    printf("Politica de Mapeamento: %s\n",
           (c->mapping == MAP_DIRECT) ? "Direto" :
           (c->mapping == MAP_FULLY_ASSOC) ? "Totalmente Associativo" :
           "Conjunto-Associativo (2 vias)");
    if (c->mapping != MAP_DIRECT) {
        printf("Politica de Substituicao: %s\n",
               (c->replacement == REP_FIFO) ? "FIFO" :
               (c->replacement == REP_LRU) ? "LRU" :
               "Random");
    } else {
        printf("Politica de Substituicao: (nao se aplica ao Direto)\n");
    }

    printf("Hits:   %d\n", c->hits);
    printf("Misses: %d\n", c->misses);
    int total = c->hits + c->misses;
    double hitRate = (total > 0) ? (100.0 * c->hits / total) : 0.0;
    printf("Taxa de acerto: %.2f%%\n", hitRate);
}

static inline int computeSetIndex(const Cache* c, int tag) {
    // Direto: numSets == numLines, setSize == 1, index = tag % numSets
    // Totalmente Associativo: numSets == 1, index = 0
    // Conjunto-Associativo: numSets = numLines / setSize, index = tag % numSets
    return tag % c->numSets;
}

int findLine(const CacheSet* set, int ways, int tag) {
    for (int i = 0; i < ways; ++i) {
        if (set->lines[i].valid && set->lines[i].tag == tag) {
            return i;
        }
    }
    return -1;
}

int findEmpty(const CacheSet* set, int ways) {
    for (int i = 0; i < ways; ++i) {
        if (!set->lines[i].valid) return i;
    }
    return -1;
}

int chooseVictimFIFO(const CacheSet* set, int ways) {
    // Menor fifoSeq -> mais antigo
    unsigned long bestSeq = (unsigned long)(-1);
    int victim = 0;
    for (int i = 0; i < ways; ++i) {
        if (!set->lines[i].valid) return i;
        if (set->lines[i].fifoSeq < bestSeq) {
            bestSeq = set->lines[i].fifoSeq;
            victim = i;
        }
    }
    return victim;
}

int chooseVictimLRU(const CacheSet* set, int ways) {
    // Menor lastUsed -> menos recentemente usado
    unsigned long best = (unsigned long)(-1);
    int victim = 0;
    for (int i = 0; i < ways; ++i) {
        if (!set->lines[i].valid) return i;
        if (set->lines[i].lastUsed < best) {
            best = set->lines[i].lastUsed;
            victim = i;
        }
    }
    return victim;
}

int chooseVictimRandom(const CacheSet* set, int ways) {
    // Se existir vazio, usa o vazio; senao escolhe aleatório entre [0, ways-1]
    int empty = findEmpty(set, ways);
    if (empty >= 0) return empty;
    return rand() % ways;
}

int chooseVictim(const Cache* c, const CacheSet* set) {
    switch (c->replacement) {
        case REP_FIFO:   return chooseVictimFIFO(set, c->setSize);
        case REP_LRU:    return chooseVictimLRU(set, c->setSize);
        case REP_RANDOM: return chooseVictimRandom(set, c->setSize);
        default:         return 0;
    }
}

void accessAddress(Cache* c, int address) {
    // Neste simulador, "tag" = valor de endereço (sem offsets/bits de bloco)
    int tag = address;

    int setIdx = computeSetIndex(c, tag);
    CacheSet* set = &c->sets[setIdx];

    c->clock++; // avança tempo lógico

    // Procura hit
    int lineIdx = findLine(set, c->setSize, tag);
    if (lineIdx >= 0) {
        // HIT
        c->hits++;
        set->lines[lineIdx].lastUsed = c->clock; // atualiza p/ LRU
        // FIFO não atualiza ordem no hit (estritamente), Random também não
        return;
    }

    // MISS
    c->misses++;

    if (c->mapping == MAP_DIRECT) {
        // Direto: sempre 1 via, a "vítima" é a única linha
        CacheLine* line = &set->lines[0];
        line->tag = tag;
        line->valid = true;
        line->lastUsed = c->clock;
        line->fifoSeq = c->clock; // para fins de debug, mantém a sequência
        return;
    }

    // Associativo ou Conjunto-Associativo: escolher vítima conforme política
    int victim = chooseVictim(c, set);
    set->lines[victim].tag = tag;
    set->lines[victim].valid = true;
    set->lines[victim].lastUsed = c->clock;
    set->lines[victim].fifoSeq = c->clock;
}

void simulate(Cache* c, const int* trace, int nTrace) {
    for (int i = 0; i < nTrace; ++i) {
        // Somente acessa valores que existem na RAM de exemplo
        int addr = trace[i];
        bool inRam = false;
        for (int k = 0; k < TAM_RAM; ++k) {
            if (RAM[k] == addr) { inRam = true; break; }
        }
        if (!inRam) continue; // ignora acessos inválidos nesta demo
        accessAddress(c, addr);
    }
}

// ======================= Menus ==============================
int menuMappingPolicy(void) {
    int option;
    printf("\nEscolha a Politica de Mapeamento:\n");
    printf("1 - Direto\n");
    printf("2 - Totalmente Associativo\n");
    printf("3 - Conjunto-Associativo (2 vias)\n");
    printf("Opcao: ");
    if (scanf("%d", &option) != 1) exit(EXIT_FAILURE);
    return option;
}

int menuReplacementPolicy(void) {
    int option;
    printf("\nEscolha a Politica de Substituicao:\n");
    printf("1 - FIFO\n");
    printf("2 - LRU\n");
    printf("3 - Random\n");
    printf("Opcao: ");
    if (scanf("%d", &option) != 1) exit(EXIT_FAILURE);
    return option;
}

// ======================= main ===============================
int main(void) {
    srand((unsigned int)time(NULL));

    printf("=== Simulador de Cache ===\n");
    int mapping = menuMappingPolicy();

    int replacement = REP_FIFO;
    if (mapping == MAP_FULLY_ASSOC || mapping == MAP_SET_ASSOC) {
        replacement = menuReplacementPolicy();
    } else {
        printf("\n(Mapeamento Direto nao requer politica de substituicao.)\n");
    }

    // Configuracao padrão: 8 linhas totais
    // Direto: 8 conjuntos x 1 via
    // Totalmente associativo: 1 conjunto x 8 vias
    // Conjunto-associativo: 4 conjuntos x 2 vias
    Cache* cache = createCache(8, (MappingPolicy)mapping, (ReplacementPolicy)replacement);

    // Trace de exemplo (edite à vontade)
    int trace[] = {1,5,10,1,20,15,5,10,20,1,5,5,15,10,20,1,15,10,5,20};
    int nTrace = sizeof(trace)/sizeof(int);

    printf("\n--- Iniciando Simulacao ---\n");
    simulate(cache, trace, nTrace);
    printStats(cache);

    freeCache(cache);
    return 0;
}
