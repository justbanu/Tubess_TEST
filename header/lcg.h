#ifndef LCG_H
#define LCG_H

#include "config.h"

typedef struct {
    unsigned long state;
} LCG;

void          lcgInit(LCG *lcg);
void          lcgSeed(LCG *lcg, unsigned long seed);
unsigned long lcgNext(LCG *lcg);
/* Return nilai dalam range [0, max) */
int           lcgRandInt(LCG *lcg, int max);

#endif /* LCG_H */