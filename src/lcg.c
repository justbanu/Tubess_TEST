#include "../header/lcg.h"

#define LCG_A  1664525UL
#define LCG_C  1013904223UL
#define LCG_M  4294967296UL

void lcgInit(LCG *lcg) {
    lcg->state = 12345678UL;
}

void lcgSeed(LCG *lcg, unsigned long seed) {
    lcg->state = seed;
}

unsigned long lcgNext(LCG *lcg) {
    lcg->state = (LCG_A * lcg->state + LCG_C) % LCG_M;
    return lcg->state;
}

int lcgRandInt(LCG *lcg, int max) {
    if (max <= 0) return 0;
    return (int)(lcgNext(lcg) % (unsigned long)max);
}