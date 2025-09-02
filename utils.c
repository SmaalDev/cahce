#include <stdio.h>
#include "utils.h"

int menuMappingPolicy(void) {
    int opt;
    printf("1 - Direto\n2 - Totalmente Associativo\n3 - Conjunto-Associativo\nOpcao: ");
    scanf("%d",&opt);
    return opt;
}

int menuReplacementPolicy(void) {
    int opt;
    printf("1 - FIFO\n2 - LRU\n3 - Random\nOpcao: ");
    scanf("%d",&opt);
    return opt;
}
