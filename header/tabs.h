#ifndef TABS_H
#define TABS_H

#include "config.h"
#include "adt_stack.h"

/* ========== Manajemen Tab ========== */

typedef struct {
    char     name[16];       /* TAB1, TAB2, ... */
    int      active;         /* 1 = dipakai, 0 = kosong */
    NavStack nav;            /* back/forward history */
} Tab;

typedef struct {
    Tab  tabs[TABS_MAX_AMOUNT];
    int  count;              /* jumlah tab aktif */
    int  current_idx;        /* indeks tab yang sedang aktif */
    int  counter;            /* counter auto-increment nama tab */
} TabManager;

void  initTabManager(TabManager *tm);
int   newTab(TabManager *tm);           /* return idx tab baru, -1 jika penuh */
int   closeTab(TabManager *tm);         /* return 1 sukses, 0 gagal */
int   prevTab(TabManager *tm, int dist);/* return 1 sukses */
int   nextTab(TabManager *tm, int dist);/* return 1 sukses */
Tab*  getCurrentTab(TabManager *tm);
int   getTabCount(TabManager *tm);
/* Mendapatkan tab aktif index relatif ke list aktif (bukan array slot) */
int   getCurrentTabDisplayIndex(TabManager *tm);

#endif /* TABS_H */