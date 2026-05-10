#ifndef ADT_MAP_H
#define ADT_MAP_H

#include "config.h"

/* ========== ADT MAP (untuk Cache) ==========
 * Menyimpan pasangan key (URL) -> value (content).
 * Implementasi menggunakan array circular dengan kebijakan FIFO eviction.
 */

typedef struct {
    char url[MAX_URL_LEN];
    char content[MAX_CONTENT_LEN];
    int  valid;
} MapEntry;

typedef struct {
    MapEntry entries[CACHE_MAX_AMOUNT];
    int      size;      /* jumlah entry valid */
    int      head;      /* index FIFO head untuk eviction */
    int      tail;      /* index FIFO tail untuk insert berikutnya */
    int      capacity;
} Map;

void  initMap(Map *m, int capacity);
int   mapGet(Map *m, const char *url, char *out_content);   /* return 1 jika hit */
int   mapPut(Map *m, const char *url, const char *content); /* return 1 jika evict terjadi */
void  mapRemove(Map *m, const char *url);
int   mapContains(Map *m, const char *url);
void  mapClear(Map *m);

#endif /* ADT_MAP_H */