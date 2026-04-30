#ifndef LCG_H
#define LCG_H

#include "config.h"

/* ========== Linear Congruential Generator ==========
 * X_{n+1} = (a * X_n + c) mod m
 * a = LCG_MULTIPLIER = 1103515245
 * c = LCG_INCREMENT  = 12345
 * m = LCG_MODULUS    = 2^31
 */

typedef struct {
    unsigned long state;
} LCG;

void          lcgInit(LCG *lcg);
void          lcgSeed(LCG *lcg, unsigned long seed);
unsigned long lcgNext(LCG *lcg);
/* Return nilai dalam range [0, max) */
int           lcgRandInt(LCG *lcg, int max);

#endif /* LCG_H */