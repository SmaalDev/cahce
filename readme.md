compile com: gcc main.c cache.c mapping.c replacement.c utils.c -o simulator.exe

execute: .\simulator.exe

A ideia é imitar o funcionamento de uma memória cache real: acessamos endereços, verificamos se eles estão na cache, contamos hits (acertos) e misses (falhas), e, quando necessário, aplicamos uma política de substituição para decidir qual linha da cache será sobrescrita.

📌 Estrutura do programa
1. RAM simulada
static int RAM[] = {1, 5, 10, 15, 20};


É só um vetor de números que representam os endereços válidos da RAM.
Se tentarmos acessar um valor fora desse vetor, o simulador ignora.

2. Políticas
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


Mapeamento: direto, totalmente associativo ou conjunto-associativo.

Substituição: FIFO (primeiro a entrar), LRU (menos usado recentemente) ou Random (linha aleatória).

3. Estruturas principais
typedef struct {
    int tag;
    bool valid;
    unsigned long lastUsed;
    unsigned long fifoSeq;
} CacheLine;

typedef struct {
    CacheLine* lines;  // vetor de "vias"
} CacheSet;

typedef struct {
    int numLines, setSize, numSets;
    MappingPolicy mapping;
    ReplacementPolicy replacement;

    CacheSet* sets;
    unsigned long clock; // tempo lógico
    int hits, misses;
} Cache;


CacheLine → representa uma linha da cache (um “slot”).
Tem a tag (identificador do endereço), um bit valid, e metadados para FIFO/LRU.

CacheSet → um conjunto (grupo de linhas).

Cache → a cache inteira (conjuntos + políticas + estatísticas).

4. Criação da cache
Cache* createCache(int numLines, MappingPolicy mapping, ReplacementPolicy replacement);


Aqui decidimos:

Mapeamento direto → 1 via por conjunto, numSets = numLines.

Totalmente associativo → 1 conjunto, setSize = numLines.

Conjunto-associativo → fixei como 2 vias (padrão), numSets = numLines / 2.

Cada conjunto recebe um vetor de linhas inicializadas como inválidas.

5. Simulação de acesso
void accessAddress(Cache* c, int address);


Passos:

Converte o endereço em tag (aqui simplificado: a tag é o próprio valor).

Descobre o conjunto (setIdx = tag % numSets).

Procura o endereço nas linhas do conjunto:

Se encontrar → HIT (atualiza lastUsed se for LRU).

Se não encontrar → MISS.

Se cache direta → substitui a única linha do conjunto.

Caso contrário → escolhe uma vítima (FIFO, LRU ou Random) e sobrescreve.

6. Políticas de substituição

FIFO → linha com menor fifoSeq (mais antiga).

LRU → linha com menor lastUsed (menos usada recentemente).

Random → sorteia uma linha do conjunto.

7. Execução da simulação
void simulate(Cache* c, const int* trace, int nTrace);


Recebe um vetor de acessos (trace) e, para cada endereço:

Verifica se existe na RAM.

Acessa via accessAddress.

Atualiza hits/misses.

8. Exemplo de execução

Entrada no main:

int trace[] = {1,5,10,1,20,15,5,10,20,1,5,5,15,10,20,1,15,10,5,20};


Saída típica:

=== Simulador de Cache ===
Escolha a Politica de Mapeamento:
1 - Direto
2 - Totalmente Associativo
3 - Conjunto-Associativo (2 vias)
Opcao: 2

Escolha a Politica de Substituicao:
1 - FIFO
2 - LRU
3 - Random
Opcao: 2

--- Iniciando Simulacao ---

=== Resultados da Simulacao ===
Linhas totais: 8 | Conjuntos: 1 | Vias por conjunto: 8
Politica de Mapeamento: Totalmente Associativo
Politica de Substituicao: LRU
Hits:   11
Misses: 9
Taxa de acerto: 55.00%

📊 Resumindo

O usuário escolhe: tipo de mapeamento e política de substituição.

A cache é criada conforme as regras do mapeamento.

O programa percorre uma lista de acessos (trace).

Para cada acesso: procura na cache → hit ou miss.

Se miss: aplica política de substituição e carrega o endereço.

No final: imprime estatísticas.

Quer que eu desenhe um passo a passo gráfico mostrando como os acessos entram e saem da cache (tipo “estado da cache a cada acesso”)? Isso ajuda muito a visualizar o funcionamento.
