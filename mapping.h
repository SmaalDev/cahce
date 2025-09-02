#ifndef MAPPING_H
#define MAPPING_H

#include "cache.h"

void simulate(Cache* c, const int* trace, int nTrace);
void accessAddress(Cache* c, int address);

#endif
