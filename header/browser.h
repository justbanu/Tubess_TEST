#ifndef BROWSER_H
#define BROWSER_H

#include "config.h"
#include "adt_simple.h"
#include "adt_set.h"
#include "adt_map.h"
#include "adt_queue.h"
#include "adt_graph.h"
#include "adt_list.h"
#include "tabs.h"
#include "lcg.h"

/* ========== Browser State ========== */
typedef struct {
    WebDatabase    db;          /* database utama halaman web */
    LinkedPagesDB  ldb;         /* relasi linked pages */
    Set            urlSet;      /* ADT Set untuk lookup cepat */
    Map            cache;       /* ADT Map untuk caching */
    DownloadQueue  dlQueue;     /* ADT Queue untuk download manager */
    Graph          webGraph;    /* ADT Graph untuk web graph */
    TabManager     tabMgr;      /* manajemen tab */
    LCG            rng;         /* random number generator */

    /* State saat ini */
    char  current_url[MAX_URL_LEN]; /* URL halaman yang sedang terbuka */
    int   page_open;                /* 1 = halaman sedang terbuka */
    int   current_linked[MAX_LINKED_PAGES]; /* page_id linked pages halaman aktif */
    int   current_linked_count;
} Browser;

/* ========== Inisialisasi ========== */
void browserInit(Browser *b);

/* ========== Helper ========== */
void browserOpenPage(Browser *b, const char *url, int add_to_history);
void browserShowPage(Browser *b, const char *url, const char *content);
int  browserCalcTicks(const char *url);

/* ========== Command Handlers ========== */
void cmdDiscover(Browser *b);
void cmdSearch(Browser *b, const char *query);
void cmdOpen(Browser *b, const char *url);
void cmdOpenLinked(Browser *b, int idx);
void cmdHome(Browser *b);
void cmdBack(Browser *b, int steps);
void cmdForward(Browser *b, int steps);
void cmdViewTabHistory(Browser *b);
void cmdNewTab(Browser *b);
void cmdCloseTab(Browser *b);
void cmdCheckTab(Browser *b);
void cmdPrevTab(Browser *b, int dist);
void cmdNextTab(Browser *b, int dist);
void cmdAddPage(Browser *b, const char *url);
void cmdEditPage(Browser *b, const char *url);
void cmdDeletePage(Browser *b, const char *url);
void cmdDownload(Browser *b, const char *url);
void cmdTick(Browser *b);
void cmdExit(Browser *b);

/* ========== Command Parser ========== */
void runBrowser(Browser *b);

#endif /* BROWSER_H */
