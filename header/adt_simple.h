#ifndef ADT_SIMPLE_H
#define ADT_SIMPLE_H

#include "config.h"

/* ========== ADT SEDERHANA: WebPage ========== */
typedef struct {
    int    id;
    char   url[MAX_URL_LEN];
    char   content[MAX_CONTENT_LEN];
    int    active;  /* 1 = ada, 0 = dihapus */
} WebPage;

/* ========== ADT SEDERHANA: LinkedPageRelation ========== */
typedef struct {
    int id;
    int id_sumber;
    int id_tujuan;
    int active;
} LinkedPageRelation;

/* ========== Database Halaman Web (Array-based) ========== */
typedef struct {
    WebPage pages[MAX_WEB_PAGES];
    int     count;        /* jumlah halaman aktif */
    int     total;        /* total slot terisi (termasuk deleted) */
    int     next_id;      /* auto-increment counter */
} WebDatabase;

/* ========== Linked Pages Database ========== */
typedef struct {
    LinkedPageRelation relations[MAX_WEB_PAGES * 5];
    int count;
    int next_id;
} LinkedPagesDB;

/* ---- Operasi WebDatabase ---- */
void     initWebDatabase(WebDatabase *db);
int      addWebPage(WebDatabase *db, const char *url, const char *content);
int      deleteWebPage(WebDatabase *db, const char *url);
WebPage* findWebPageByUrl(WebDatabase *db, const char *url);
WebPage* findWebPageById(WebDatabase *db, int id);
int      urlExists(WebDatabase *db, const char *url);
void     editWebPage(WebDatabase *db, const char *url, const char *newContent);

/* ---- Binary Search (URL harus sudah diurutkan) ---- */
/* Mengembalikan indeks di sorted_indices, atau -1 jika tidak ada */
int      binarySearchUrl(WebDatabase *db, int *sorted_indices, int sorted_count, const char *url);

/* ---- Sort & Filter ---- */
/* Mengisi sorted_indices dengan indeks pages yang aktif, urut lexicographic */
int      getSortedUrlIndices(WebDatabase *db, int *out_indices);

/* ---- Filter: ambil halaman yang URL-nya diawali prefix ---- */
int      filterByPrefix(WebDatabase *db, const char *prefix, int *out_indices);

/* ---- Operasi LinkedPagesDB ---- */
void     initLinkedPagesDB(LinkedPagesDB *ldb);
int      addLinkedPage(LinkedPagesDB *ldb, int id_sumber, int id_tujuan);
void     removeLinksForPage(LinkedPagesDB *ldb, int page_id);
/* Mengisi out_ids dengan id_tujuan dari id_sumber, return jumlah linked */
int      getLinkedPages(LinkedPagesDB *ldb, int id_sumber, int *out_ids);

#endif /* ADT_SIMPLE_H */