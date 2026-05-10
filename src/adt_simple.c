#include <string.h>
#include <stdlib.h>
#include "../header/adt_simple.h"

/* ========== WebDatabase ========== */

void initWebDatabase(WebDatabase *db) {
    db->count   = 0;
    db->total   = 0;
    db->next_id = 1;
    for (int i = 0; i < MAX_WEB_PAGES; i++) {
        db->pages[i].active = 0;
        db->pages[i].id     = 0;
    }
}

int addWebPage(WebDatabase *db, const char *url, const char *content) {
    if (db->total >= MAX_WEB_PAGES) return -1;
    if (findWebPageByUrl(db, url)) return -2; /* sudah ada */
    int slot = db->total;
    db->pages[slot].id = db->next_id++;
    strncpy(db->pages[slot].url, url, MAX_URL_LEN - 1);
    db->pages[slot].url[MAX_URL_LEN - 1] = '\0';
    strncpy(db->pages[slot].content, content, MAX_CONTENT_LEN - 1);
    db->pages[slot].content[MAX_CONTENT_LEN - 1] = '\0';
    db->pages[slot].active = 1;
    db->total++;
    db->count++;
    return db->pages[slot].id;
}

int deleteWebPage(WebDatabase *db, const char *url) {
    for (int i = 0; i < db->total; i++) {
        if (db->pages[i].active && strcmp(db->pages[i].url, url) == 0) {
            db->pages[i].active = 0;
            db->count--;
            return 1;
        }
    }
    return 0;
}

WebPage* findWebPageByUrl(WebDatabase *db, const char *url) {
    for (int i = 0; i < db->total; i++) {
        if (db->pages[i].active && strcmp(db->pages[i].url, url) == 0) {
            return &db->pages[i];
        }
    }
    return NULL;
}

WebPage* findWebPageById(WebDatabase *db, int id) {
    for (int i = 0; i < db->total; i++) {
        if (db->pages[i].active && db->pages[i].id == id) {
            return &db->pages[i];
        }
    }
    return NULL;
}

int urlExists(WebDatabase *db, const char *url) {
    return findWebPageByUrl(db, url) != NULL;
}

void editWebPage(WebDatabase *db, const char *url, const char *newContent) {
    WebPage *p = findWebPageByUrl(db, url);
    if (!p) return;
    strncpy(p->content, newContent, MAX_CONTENT_LEN - 1);
    p->content[MAX_CONTENT_LEN - 1] = '\0';
}

/* ========== Sort & Filter (Array Search) ========== */

/* Comparator untuk qsort berdasarkan URL lexicographic */
static WebDatabase *_sort_db = NULL;

static int compareUrlIdx(const void *a, const void *b) {
    int ia = *(int*)a;
    int ib = *(int*)b;
    return strcmp(_sort_db->pages[ia].url, _sort_db->pages[ib].url);
}

/* Mengisi sorted_indices dengan indeks pages aktif, urut lexicographic.
   Return jumlah halaman aktif. */
int getSortedUrlIndices(WebDatabase *db, int *out_indices) {
    int count = 0;
    for (int i = 0; i < db->total; i++) {
        if (db->pages[i].active) {
            out_indices[count++] = i;
        }
    }
    _sort_db = db;
    qsort(out_indices, count, sizeof(int), compareUrlIdx);
    return count;
}

/* Binary Search pada sorted_indices.
   Mencari url di antara db->pages[sorted_indices[0..sorted_count-1]].
   Return indeks di sorted_indices, atau -1 jika tidak ditemukan. */
int binarySearchUrl(WebDatabase *db, int *sorted_indices, int sorted_count, const char *url) {
    int lo = 0, hi = sorted_count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        int cmp = strcmp(db->pages[sorted_indices[mid]].url, url);
        if (cmp == 0) return mid;
        else if (cmp < 0) lo = mid + 1;
        else              hi = mid - 1;
    }
    return -1;
}

/* Filter: kembalikan indeks pages yang URL-nya diawali prefix.
   Return jumlah yang ditemukan. */
int filterByPrefix(WebDatabase *db, const char *prefix, int *out_indices) {
    int count = 0;
    int plen  = (int)strlen(prefix);
    for (int i = 0; i < db->total; i++) {
        if (db->pages[i].active && strncmp(db->pages[i].url, prefix, plen) == 0) {
            out_indices[count++] = i;
        }
    }
    return count;
}

/* ========== LinkedPagesDB ========== */

void initLinkedPagesDB(LinkedPagesDB *ldb) {
    ldb->count   = 0;
    ldb->next_id = 1;
    for (int i = 0; i < MAX_WEB_PAGES * 5; i++) {
        ldb->relations[i].active = 0;
    }
}

int addLinkedPage(LinkedPagesDB *ldb, int id_sumber, int id_tujuan) {
    if (ldb->count >= MAX_WEB_PAGES * 5) return -1;
    /* Cari slot kosong */
    for (int i = 0; i < MAX_WEB_PAGES * 5; i++) {
        if (!ldb->relations[i].active) {
            ldb->relations[i].id        = ldb->next_id++;
            ldb->relations[i].id_sumber = id_sumber;
            ldb->relations[i].id_tujuan = id_tujuan;
            ldb->relations[i].active    = 1;
            ldb->count++;
            return ldb->relations[i].id;
        }
    }
    return -1;
}

void removeLinksForPage(LinkedPagesDB *ldb, int page_id) {
    for (int i = 0; i < MAX_WEB_PAGES * 5; i++) {
        if (ldb->relations[i].active &&
            (ldb->relations[i].id_sumber == page_id ||
             ldb->relations[i].id_tujuan == page_id)) {
            ldb->relations[i].active = 0;
            ldb->count--;
        }
    }
}

int getLinkedPages(LinkedPagesDB *ldb, int id_sumber, int *out_ids) {
    int count = 0;
    for (int i = 0; i < MAX_WEB_PAGES * 5; i++) {
        if (ldb->relations[i].active && ldb->relations[i].id_sumber == id_sumber) {
            out_ids[count++] = ldb->relations[i].id_tujuan;
        }
    }
    return count;
}