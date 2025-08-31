#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ======================= Prototypes =========================
void startSimulation(int mappingPolicy, int replacementPolicy);

void directMapping();
void associativeMapping();
void setAssociativeMapping();

void replacementFIFO();
void replacementLRU();
void replacementRandom();

int menuMappingPolicy();
int menuReplacementPolicy();

// =============================================================

int main() {
    int mappingPolicy, replacementPolicy;

    printf("=== Cache Simulator ===\n");

    mappingPolicy = menuMappingPolicy();
    replacementPolicy = menuReplacementPolicy();

    startSimulation(mappingPolicy, replacementPolicy);

    return 0;
}

// =============================================================
// Functions Implementation

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

void startSimulation(int mappingPolicy, int replacementPolicy) {
    printf("\n--- Starting Simulation ---\n");

    // Escolher política de mapeamento
    if (mappingPolicy == 1) {
        directMapping();
    }
    else {
        if (mappingPolicy == 2) {
            associativeMapping();
        }
        else {
            if (mappingPolicy == 3) {
                setAssociativeMapping();
            }
            else {
                printf("Invalid mapping policy!\n");
                return;
            }
        }
    }

    // Escolher política de substituição
    if (replacementPolicy == 1) {
        replacementFIFO();
    }
    else {
        if (replacementPolicy == 2) {
            replacementLRU();
        }
        else {
            if (replacementPolicy == 3) {
                replacementRandom();
            }
            else {
                printf("Invalid replacement policy!\n");
                return;
            }
        }
    }
}

void directMapping() {
    printf(">> Using Direct Mapping...\n");
    // TODO: Implementar a lógica
}

void associativeMapping() {
    printf(">> Using Fully Associative Mapping...\n");
    // TODO: Implementar a lógica
}

void setAssociativeMapping() {
    printf(">> Using Set-Associative Mapping...\n");
    // TODO: Implementar a lógica
}

void replacementFIFO() {
    printf(">> Replacement Policy: FIFO\n");
    // TODO: Implementar a lógica
}

void replacementLRU() {
    printf(">> Replacement Policy: LRU\n");
    // TODO: Implementar a lógica
}

void replacementRandom() {
    printf(">> Replacement Policy: Random\n");
    srand(time(NULL));
    // TODO: Implementar a lógica
}
