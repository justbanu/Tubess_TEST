#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/browser.h"
#include "../header/data.h"

/* ================================================================
   INISIALISASI BROWSER
   ================================================================ */

void browserInit(Browser *b) {
    initWebDatabase(&b->db);
    initLinkedPagesDB(&b->ldb);
    initSet(&b->urlSet);
    initMap(&b->cache, CACHE_MAX_AMOUNT);
    initQueue(&b->dlQueue, DOWNLOAD_MAX_AMOUNT);
    initGraph(&b->webGraph);
    initTabManager(&b->tabMgr);
    lcgInit(&b->rng);

    b->current_url[0]       = '\0';
    b->page_open            = 0;
    b->current_linked_count = 0;

    initData(&b->db, &b->ldb, &b->urlSet, &b->webGraph);
}

/* ================================================================
   HELPER: TAMPILKAN HALAMAN
   ================================================================ */

int browserCalcTicks(const char *url) {
    int len = (int)strlen(url);
    return len / 5 + 2;
}

static int isDigitChar(char c) {
    return c >= '0' && c <= '9';
}

static int isAlphaChar(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static int isAlnumChar(char c) {
    return isAlphaChar(c) || isDigitChar(c);
}

/* E01: ^(?!-)[A-Za-z0-9-]{1,63}(?<!-)(\.[A-Za-z]{2,})+$ */
int isValidUrl(const char *url) {
    int len = (int)strlen(url);
    if (len == 0) return 0;
    if (url[0] == '-') return 0;
    int i = 0;
    while (i < len && url[i] != '.') {
        char c = url[i];
        if (!isAlnumChar(c) && c != '-') return 0;
        i++;
    }
    if (i == 0 || i > 63) return 0;
    if (url[i - 1] == '-') return 0;
    if (i >= len) return 0;
    while (i < len) {
        if (url[i] != '.') return 0;
        i++;
        int tld_start = i;
        while (i < len && url[i] != '.') {
            if (!isAlphaChar(url[i])) return 0;
            i++;
        }
        if (i - tld_start < 2) return 0;
    }
    return 1;
}

/* E02: bilangan bulat positif, max 6 digit (max 999999) */
int isValidNumeric(const char *s) {
    if (!s || s[0] == '\0') return 0;
    int len = 0;
    for (int i = 0; s[i]; i++) {
        if (!isDigitChar(s[i])) return 0;
        len++;
    }
    if (len == 0 || len > 6) return 0;
    int val = atoi(s);
    return (val > 0 && val <= 999999);
}

void browserShowPage(Browser *b, const char *url, const char *content) {
    printf("\n");
    printf(COLOR_CYAN "╔══════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║ " COLOR_BOLD "%-38s" COLOR_RESET COLOR_CYAN " ║\n" COLOR_RESET, url);
    printf(COLOR_CYAN "╚══════════════════════════════════════╝\n" COLOR_RESET);
    printf("%s\n", content);

    /* Tampilkan linked pages */
    WebPage *page = findWebPageByUrl(&b->db, url);
    if (page) {
        int neighbor_ids[MAX_LINKED_PAGES];
        int ncount = graphGetNeighbors(&b->webGraph, page->id, neighbor_ids);
        /* Fallback ke ldb jika graph belum sinkron */
        if (ncount == 0) {
            ncount = getLinkedPages(&b->ldb, page->id, neighbor_ids);
        }
        if (ncount > 0) {
            printf("\n" COLOR_YELLOW "Linked pages:\n" COLOR_RESET);
            for (int i = 0; i < ncount; i++) {
                WebPage *linked = findWebPageById(&b->db, neighbor_ids[i]);
                if (linked) {
                    printf("  " COLOR_BLUE "[%d]" COLOR_RESET " %s\n", i + 1, linked->url);
                }
            }
            /* Simpan state linked pages untuk openlinked */
            b->current_linked_count = ncount;
            for (int i = 0; i < ncount; i++) {
                b->current_linked[i] = neighbor_ids[i];
            }
        } else {
            printf("\n" COLOR_DIM "(Tidak ada linked pages)\n" COLOR_RESET);
            b->current_linked_count = 0;
        }
    }
    printf("\n");
}

/* Membuka halaman: cek cache, cari di DB, update history */
void browserOpenPage(Browser *b, const char *url, int add_to_history) {
    char content[MAX_CONTENT_LEN];

    /* Cek cache */
    if (mapGet(&b->cache, url, content)) {
        printf(COLOR_GREEN "[Status: Cache-Hit]" COLOR_RESET " Mengambil data dari cache...\n");
    } else {
        /* Linear search di Set lalu ambil content dari DB */
        if (!setContains(&b->urlSet, url)) {
            printf(COLOR_RED "404 Not Found!" COLOR_RESET " Halaman tidak ditemukan.\n");
            return;
        }
        printf(COLOR_YELLOW "[Status: Cache-Miss]" COLOR_RESET " Mengambil data dari database...\n");
        WebPage *page = findWebPageByUrl(&b->db, url);
        if (!page) {
            printf(COLOR_RED "404 Not Found!" COLOR_RESET " Halaman tidak ditemukan.\n");
            return;
        }
        strncpy(content, page->content, MAX_CONTENT_LEN - 1);
        content[MAX_CONTENT_LEN - 1] = '\0';
        mapPut(&b->cache, url, content);
    }

    /* Tampilkan */
    browserShowPage(b, url, content);

    /* Update state */
    strncpy(b->current_url, url, MAX_URL_LEN - 1);
    b->current_url[MAX_URL_LEN - 1] = '\0';
    b->page_open = 1;

    /* Tambahkan ke history tab */
    if (add_to_history) {
        Tab *tab = getCurrentTab(&b->tabMgr);
        navPush(&tab->nav, url);
    }
}
