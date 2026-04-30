#include <string.h>
#include <stdio.h>
#include "../header/tabs.h"

void initTabManager(TabManager *tm) {
    tm->count       = 0;
    tm->current_idx = 0;
    tm->counter     = 0;
    for (int i = 0; i < TABS_MAX_AMOUNT; i++) {
        tm->tabs[i].active = 0;
        tm->tabs[i].name[0] = '\0';
        initNavStack(&tm->tabs[i].nav);
    }
    /* Buat TAB1 default */
    tm->counter = 1;
    snprintf(tm->tabs[0].name, 16, "TAB%d", tm->counter);
    tm->tabs[0].active = 1;
    initNavStack(&tm->tabs[0].nav);
    tm->count       = 1;
    tm->current_idx = 0;
}

/* Mendapatkan slot tab aktif ke-n (0-based) dalam array */
static int getNthActiveSlot(TabManager *tm, int n) {
    int found = 0;
    for (int i = 0; i < TABS_MAX_AMOUNT; i++) {
        if (tm->tabs[i].active) {
            if (found == n) return i;
            found++;
        }
    }
    return -1;
}

/* Mendapatkan posisi display (0-based) dari current_idx */
static int getDisplayPos(TabManager *tm) {
    int pos = 0;
    for (int i = 0; i < TABS_MAX_AMOUNT; i++) {
        if (!tm->tabs[i].active) continue;
        if (i == tm->current_idx) return pos;
        pos++;
    }
    return 0;
}

int newTab(TabManager *tm) {
    if (tm->count >= TABS_MAX_AMOUNT) return -1;
    /* Cari slot kosong */
    for (int i = 0; i < TABS_MAX_AMOUNT; i++) {
        if (!tm->tabs[i].active) {
            tm->counter++;
            snprintf(tm->tabs[i].name, 16, "TAB%d", tm->counter);
            tm->tabs[i].active = 1;
            initNavStack(&tm->tabs[i].nav);
            tm->count++;
            return i;
        }
    }
    return -1;
}

int closeTab(TabManager *tm) {
    if (tm->count <= 1) return 0;
    int cur = tm->current_idx;
    tm->tabs[cur].active = 0;
    tm->count--;
    /* Pindah ke tab berikutnya atau sebelumnya */
    /* Cari tab aktif terdekat */
    for (int i = cur + 1; i < TABS_MAX_AMOUNT; i++) {
        if (tm->tabs[i].active) {
            tm->current_idx = i;
            return 1;
        }
    }
    for (int i = cur - 1; i >= 0; i--) {
        if (tm->tabs[i].active) {
            tm->current_idx = i;
            return 1;
        }
    }
    return 1;
}

int prevTab(TabManager *tm, int dist) {
    int cur_pos = getDisplayPos(tm);
    int target  = cur_pos - dist;
    if (target < 0) return 0;
    int slot = getNthActiveSlot(tm, target);
    if (slot == -1) return 0;
    tm->current_idx = slot;
    return 1;
}

int nextTab(TabManager *tm, int dist) {
    int cur_pos = getDisplayPos(tm);
    int target  = cur_pos + dist;
    if (target >= tm->count) return 0;
    int slot = getNthActiveSlot(tm, target);
    if (slot == -1) return 0;
    tm->current_idx = slot;
    return 1;
}

Tab* getCurrentTab(TabManager *tm) {
    return &tm->tabs[tm->current_idx];
}

int getTabCount(TabManager *tm) {
    return tm->count;
}

int getCurrentTabDisplayIndex(TabManager *tm) {
    return getDisplayPos(tm) + 1; /* 1-based */
}