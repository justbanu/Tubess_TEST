#include "../header/lcg.h"

void lcgInit(LCG *lcg) {
    lcg->state = LCG_SEED_INITIAL;
}

void lcgSeed(LCG *lcg, unsigned long seed) {
    lcg->state = seed;
}

unsigned long lcgNext(LCG *lcg) {
    lcg->state = ((unsigned long)LCG_MULTIPLIER * lcg->state + LCG_INCREMENT) % LCG_MODULUS;
    return lcg->state;
}

int lcgRandInt(LCG *lcg, int max) {
    if (max <= 0) return 0;
    return (int)(lcgNext(lcg) % (unsigned long)max);
}