#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../header/browser.h"

/* ================================================================
   F01 - DISCOVER
   ================================================================ */

void cmdDiscover(Browser *b) {
    /* Kumpulkan semua indeks aktif */
    int indices[MAX_WEB_PAGES];
    int count = 0;
    for (int i = 0; i < b->db.total; i++) {
        if (b->db.pages[i].active) {
            indices[count++] = i;
        }
    }

    if (count == 0) {
        printf(COLOR_YELLOW "Tidak ada halaman web yang tersedia.\n" COLOR_RESET);
        return;
    }

    int show = (count < 5) ? count : 5;

    /* Pilih 'show' elemen acak unik menggunakan LCG (Fisher-Yates partial shuffle) */
    int chosen[MAX_WEB_PAGES];
    for (int i = 0; i < count; i++) chosen[i] = indices[i];
    for (int i = 0; i < show; i++) {
        int j = i + lcgRandInt(&b->rng, count - i);
        int tmp = chosen[i]; chosen[i] = chosen[j]; chosen[j] = tmp;
    }

    printf(COLOR_CYAN "\n╔══ DISCOVER ══════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║ " COLOR_RESET "Berikut halaman yang mungkin menarik:\n");
    for (int i = 0; i < show; i++) {
        printf("  " COLOR_BLUE "→" COLOR_RESET " %s\n", b->db.pages[chosen[i]].url);
    }
    printf(COLOR_CYAN "╚══════════════════════════════════════╝\n" COLOR_RESET);
}

/* ================================================================
   F02 - SEARCH
   ================================================================ */

void cmdSearch(Browser *b, const char *query) {
    /* Filter berdasarkan prefix (F02 dasar: cocok dari awal URL) */
    int out_indices[MAX_WEB_PAGES];
    int count = filterByPrefix(&b->db, query, out_indices);

    printf(COLOR_CYAN "\n╔══ SEARCH ════════════════════════════╗\n" COLOR_RESET);
    printf("  Search result(s) untuk " COLOR_BOLD "\"%s\"" COLOR_RESET ":\n", query);

    if (count == 0) {
        printf("  " COLOR_RED "Tidak Ditemukan.\n" COLOR_RESET);
    } else {
        /* Urutkan hasil search secara lexicographic */
        /* (Array Sort) */
        for (int i = 0; i < count - 1; i++) {
            for (int j = i + 1; j < count; j++) {
                if (strcmp(b->db.pages[out_indices[i]].url,
                           b->db.pages[out_indices[j]].url) > 0) {
                    int tmp = out_indices[i];
                    out_indices[i] = out_indices[j];
                    out_indices[j] = tmp;
                }
            }
        }
        for (int i = 0; i < count; i++) {
            printf("  " COLOR_BLUE "→" COLOR_RESET " %s\n", b->db.pages[out_indices[i]].url);
        }

        /* Binary search demo: cek apakah query sendiri ada sebagai URL exact */
        int sorted_all[MAX_WEB_PAGES];
        int sorted_count = getSortedUrlIndices(&b->db, sorted_all);
        int bs_result = binarySearchUrl(&b->db, sorted_all, sorted_count, query);
        if (bs_result != -1) {
            printf("  " COLOR_GREEN "[Binary Search]" COLOR_RESET
                   " URL exact ditemukan: %s\n",
                   b->db.pages[sorted_all[bs_result]].url);
        }
    }
    printf(COLOR_CYAN "╚══════════════════════════════════════╝\n" COLOR_RESET);
}

/* ================================================================
   F03 - OPEN PAGE
   ================================================================ */

void cmdOpen(Browser *b, const char *url) {
    if (!isValidUrl(url)) {
        printf(COLOR_RED "URL tidak valid!\n" COLOR_RESET);
        return;
    }
    browserOpenPage(b, url, 1);
}

/* ================================================================
   F09 - OPEN LINKED
   ================================================================ */

void cmdOpenLinked(Browser *b, int idx) {
    if (!b->page_open) {
        printf(COLOR_RED "ERROR: COMMAND HANYA DAPAT DIGUNAKAN SAAT HALAMAN WEB TERBUKA!\n" COLOR_RESET);
        return;
    }
    if (b->current_linked_count == 0) {
        printf(COLOR_RED "ERROR: HALAMAN TIDAK MEMILIKI TAUTAN YANG BISA DIBUKA!\n" COLOR_RESET);
        return;
    }
    if (idx < 1 || idx > b->current_linked_count) {
        printf(COLOR_RED "ERROR: INDEKS LINKED PAGE TIDAK VALID! (1 - %d)\n" COLOR_RESET,
               b->current_linked_count);
        return;
    }
    int target_id = b->current_linked[idx - 1];
    WebPage *page = findWebPageById(&b->db, target_id);
    if (!page) {
        printf(COLOR_RED "ERROR: Halaman linked tidak ditemukan di database.\n" COLOR_RESET);
        return;
    }
    browserOpenPage(b, page->url, 1);
}

/* ================================================================
   F07 - BACK / FORWARD
   ================================================================ */

void cmdBack(Browser *b, int steps) {
    Tab *tab = getCurrentTab(&b->tabMgr);
    if (tab->nav.current < 0) {
        printf(COLOR_RED "ERROR: BACK TIDAK BISA DIJALANKAN KARENA TIDAK ADA HALAMAN SEBELUMNYA!\n" COLOR_RESET);
        return;
    }
    char url[MAX_URL_LEN];
    int ok;
    if (steps == 1) ok = navBack(&tab->nav, url);
    else            ok = navBackN(&tab->nav, steps, url);

    if (!ok) {
        printf(COLOR_RED "ERROR: BACK TIDAK BISA DIGUNAKAN LAGI KARENA TIDAK ADA HALAMAN SEBELUMNYA!\n" COLOR_RESET);
        /* Tampilkan ulang halaman saat ini */
        const char *cur = navCurrentUrl(&tab->nav);
        if (cur) browserOpenPage(b, cur, 0);
        return;
    }
    printf(COLOR_GREEN "BACK: KEMBALI KE HALAMAN %s\n" COLOR_RESET, url);
    browserOpenPage(b, url, 0);
    strncpy(b->current_url, url, MAX_URL_LEN - 1);
}

void cmdForward(Browser *b, int steps) {
    Tab *tab = getCurrentTab(&b->tabMgr);
    if (tab->nav.current < 0) {
        printf(COLOR_RED "ERROR: FORWARD TIDAK BISA DIJALANKAN KARENA TIDAK ADA HALAMAN SELANJUTNYA!\n" COLOR_RESET);
        return;
    }
    char url[MAX_URL_LEN];
    int ok;
    if (steps == 1) ok = navForward(&tab->nav, url);
    else            ok = navForwardN(&tab->nav, steps, url);

    if (!ok) {
        printf(COLOR_RED "ERROR: FORWARD TIDAK BISA DIJALANKAN KARENA TIDAK ADA HALAMAN SELANJUTNYA!\n" COLOR_RESET);
        const char *cur = navCurrentUrl(&tab->nav);
        if (cur) browserOpenPage(b, cur, 0);
        return;
    }
    printf(COLOR_GREEN "FORWARD: KE HALAMAN %s\n" COLOR_RESET, url);
    browserOpenPage(b, url, 0);
    strncpy(b->current_url, url, MAX_URL_LEN - 1);
}

/* ================================================================
   F08 - VIEW TAB HISTORY
   ================================================================ */

void cmdViewTabHistory(Browser *b) {
    Tab *tab = getCurrentTab(&b->tabMgr);
    int count = navHistoryCount(&tab->nav);

    printf(COLOR_CYAN "\n╔══ TAB HISTORY: %s ════════════════════╗\n" COLOR_RESET,
           tab->name);
    if (count == 0) {
        printf("  (Belum ada riwayat halaman.)\n");
    } else {
        char history[MAX_HISTORY_LEN][MAX_URL_LEN];
        navGetHistory(&tab->nav, history);
        for (int i = 0; i < count; i++) {
            if (i == tab->nav.current) {
                printf("  " COLOR_GREEN "[%d]" COLOR_RESET " %s " COLOR_YELLOW "<- YOU ARE HERE\n" COLOR_RESET,
                       i, history[i]);
            } else {
                printf("  " COLOR_DIM "[%d]" COLOR_RESET " %s\n", i, history[i]);
            }
        }
    }
    printf(COLOR_CYAN "╚══════════════════════════════════════╝\n" COLOR_RESET);

    /* Tampilkan ulang halaman aktif jika ada */
    if (b->page_open && b->current_url[0] != '\0') {
        char content[MAX_CONTENT_LEN];
        if (mapGet(&b->cache, b->current_url, content)) {
            browserShowPage(b, b->current_url, content);
        } else {
            WebPage *page = findWebPageByUrl(&b->db, b->current_url);
            if (page) browserShowPage(b, b->current_url, page->content);
        }
    }
}

/* ================================================================
   F06 - TABS
   ================================================================ */

void cmdHome(Browser *b) {
    b->page_open            = 0;
    b->current_url[0]       = '\0';
    b->current_linked_count = 0;
    printf(COLOR_CYAN "\n[HOME] Kembali ke menu utama.\n" COLOR_RESET);
    printf("Tab aktif: " COLOR_BOLD "%s\n" COLOR_RESET,
           getCurrentTab(&b->tabMgr)->name);
}

void cmdNewTab(Browser *b) {
    int slot = newTab(&b->tabMgr);
    if (slot == -1) {
        printf(COLOR_RED "ERROR: Jumlah tab tidak bisa melebihi batas maksimum (%d)!\n" COLOR_RESET,
               TABS_MAX_AMOUNT);
    } else {
        printf(COLOR_GREEN "Tab baru (" COLOR_BOLD "%s" COLOR_RESET COLOR_GREEN ") berhasil dibuat!\n" COLOR_RESET,
               b->tabMgr.tabs[slot].name);
    }
}

void cmdCloseTab(Browser *b) {
    if (b->tabMgr.count <= 1) {
        printf(COLOR_RED "ERROR: Tidak bisa menutup tab, tab minimal berjumlah 1!\n" COLOR_RESET);
        return;
    }
    char name[16];
    strncpy(name, getCurrentTab(&b->tabMgr)->name, 15);
    closeTab(&b->tabMgr);
    printf(COLOR_GREEN "%s berhasil ditutup.\n" COLOR_RESET, name);
    printf("Tab aktif sekarang: " COLOR_BOLD "%s\n" COLOR_RESET,
           getCurrentTab(&b->tabMgr)->name);
    /* Reset page state karena ganti tab */
    b->page_open            = 0;
    b->current_url[0]       = '\0';
    b->current_linked_count = 0;
}

void cmdCheckTab(Browser *b) {
    TabManager *tm = &b->tabMgr;
    printf(COLOR_CYAN "\n╔══ DAFTAR TAB ════════════════════════╗\n" COLOR_RESET);
    int pos = 1;
    for (int i = 0; i < TABS_MAX_AMOUNT; i++) {
        if (!tm->tabs[i].active) continue;
        if (i == tm->current_idx) {
            printf("  " COLOR_GREEN "[%d]" COLOR_RESET " " COLOR_BOLD "%s" COLOR_RESET
                   COLOR_YELLOW " ◄ AKTIF\n" COLOR_RESET,
                   pos, tm->tabs[i].name);
        } else {
            printf("  [%d] %s\n", pos, tm->tabs[i].name);
        }
        pos++;
    }
    printf(COLOR_CYAN "╚══════════════════════════════════════╝\n" COLOR_RESET);
    printf("Current tab: " COLOR_BOLD "%s\n" COLOR_RESET,
           getCurrentTab(tm)->name);
}

void cmdPrevTab(Browser *b, int dist) {
    char prev_name[16];
    strncpy(prev_name, getCurrentTab(&b->tabMgr)->name, 15);
    if (!prevTab(&b->tabMgr, dist)) {
        printf(COLOR_RED "ERROR: Posisi tab tidak valid!\n" COLOR_RESET);
        return;
    }
    printf(COLOR_GREEN "Tab saat ini berhasil diganti ke %s.\n" COLOR_RESET,
           getCurrentTab(&b->tabMgr)->name);
    /* Reset page state karena ganti tab */
    b->page_open            = 0;
    b->current_url[0]       = '\0';
    b->current_linked_count = 0;
    /* Tampilkan halaman terakhir tab ini jika ada */
    Tab *tab = getCurrentTab(&b->tabMgr);
    const char *last = navCurrentUrl(&tab->nav);
    if (last && last[0] != '\0') {
        printf(COLOR_DIM "(Melanjutkan di: %s)\n" COLOR_RESET, last);
        browserOpenPage(b, last, 0);
    }
}

void cmdNextTab(Browser *b, int dist) {
    if (!nextTab(&b->tabMgr, dist)) {
        printf(COLOR_RED "ERROR: Posisi tab tidak valid!\n" COLOR_RESET);
        return;
    }
    printf(COLOR_GREEN "Tab saat ini berhasil diganti ke %s.\n" COLOR_RESET,
           getCurrentTab(&b->tabMgr)->name);
    /* Reset page state */
    b->page_open            = 0;
    b->current_url[0]       = '\0';
    b->current_linked_count = 0;
    Tab *tab = getCurrentTab(&b->tabMgr);
    const char *last = navCurrentUrl(&tab->nav);
    if (last && last[0] != '\0') {
        printf(COLOR_DIM "(Melanjutkan di: %s)\n" COLOR_RESET, last);
        browserOpenPage(b, last, 0);
    }
}

/* ================================================================
   F05 - PAGE MANAGEMENT
   ================================================================ */

static void readMultilineInput(char *buf, int maxlen) {
    int total = 0;
    char line[512];
    while (1) {
        printf("  >>> ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;
        /* Hapus newline */
        int len = (int)strlen(line);
        if (len > 0 && line[len-1] == '\n') line[--len] = '\0';

        if (strcmp(line, ".") == 0) break;

        if (total + len + 1 < maxlen) {
            if (total > 0) {
                buf[total++] = '\n';
            }
            strncpy(buf + total, line, maxlen - total - 1);
            total += len;
            buf[total] = '\0';
        }
    }
}

void cmdAddPage(Browser *b, const char *url) {
    if (!isValidUrl(url)) {
        printf(COLOR_RED "URL tidak valid!\n" COLOR_RESET);
        return;
    }
    if (urlExists(&b->db, url)) {
        printf(COLOR_RED "Sudah terdapat halaman dengan URL %s." COLOR_RESET
               " Gunakan URL lain yang belum terdaftar!\n", url);
        return;
    }

    printf("Masukkan konten (Akhiri dengan titik '.' di baris baru):\n");
    char content[MAX_CONTENT_LEN];
    content[0] = '\0';
    readMultilineInput(content, MAX_CONTENT_LEN);

    if (content[0] == '\0') {
        printf(COLOR_RED "Konten tidak boleh kosong!\n" COLOR_RESET);
        return;
    }

    int page_id = addWebPage(&b->db, url, content);
    if (page_id < 0) {
        printf(COLOR_RED "Gagal menambahkan halaman (database penuh).\n" COLOR_RESET);
        return;
    }
    setInsert(&b->urlSet, url, page_id);
    graphAddVertex(&b->webGraph, page_id);

    /* Tambahkan linked pages */
    printf("Masukkan linked pages (Ketik 'DONE' jika sudah selesai):\n");
    char linked_url[MAX_URL_LEN];
    while (1) {
        printf("  >>> ");
        fflush(stdout);
        if (!fgets(linked_url, MAX_URL_LEN, stdin)) break;
        int len = (int)strlen(linked_url);
        if (len > 0 && linked_url[len-1] == '\n') linked_url[--len] = '\0';

        if (strcmp(linked_url, "DONE") == 0) break;

        if (!isValidUrl(linked_url)) {
            printf(COLOR_RED "  URL tidak valid!\n" COLOR_RESET);
            continue;
        }
        WebPage *target = findWebPageByUrl(&b->db, linked_url);
        if (!target) {
            printf(COLOR_RED "  URL tidak ditemukan!\n" COLOR_RESET);
            continue;
        }
        addLinkedPage(&b->ldb, page_id, target->id);
        graphAddEdge(&b->webGraph, page_id, target->id);
        printf(COLOR_GREEN "  → %s ditambahkan sebagai linked page.\n" COLOR_RESET, linked_url);
    }

    printf(COLOR_GREEN "Halaman %s berhasil ditambahkan!\n" COLOR_RESET, url);
}

void cmdEditPage(Browser *b, const char *url) {
    if (!isValidUrl(url)) {
        printf(COLOR_RED "URL tidak valid!\n" COLOR_RESET);
        return;
    }
    WebPage *page = findWebPageByUrl(&b->db, url);
    if (!page) {
        printf(COLOR_RED "Tidak ada halaman dengan URL %s!\n" COLOR_RESET, url);
        return;
    }

    /* Cek cache */
    char cached[MAX_CONTENT_LEN];
    if (mapGet(&b->cache, url, cached)) {
        printf(COLOR_GREEN "[Status: Cache-Hit]" COLOR_RESET " Mengambil data dari cache...\n");
    } else {
        printf(COLOR_YELLOW "[Status: Cache-Miss]" COLOR_RESET " Mengambil data dari database...\n");
    }

    /* Tampilkan konten saat ini */
    printf("\nKonten saat ini:\n");
    printf(COLOR_DIM "%s\n" COLOR_RESET, page->content);

    /* Tampilkan linked pages saat ini */
    int linked_ids[MAX_LINKED_PAGES];
    int lcount = graphGetNeighbors(&b->webGraph, page->id, linked_ids);
    if (lcount == 0) lcount = getLinkedPages(&b->ldb, page->id, linked_ids);
    if (lcount > 0) {
        printf("\nLinked pages:\n");
        for (int i = 0; i < lcount; i++) {
            WebPage *lp = findWebPageById(&b->db, linked_ids[i]);
            if (lp) printf("  [%d] %s\n", i+1, lp->url);
        }
    }

    /* Edit konten */
    printf("\nMasukkan konten baru (akhiri dengan '.' atau ketik '.' saja jika tidak ingin mengubah):\n");
    char new_content[MAX_CONTENT_LEN];
    new_content[0] = '\0';
    readMultilineInput(new_content, MAX_CONTENT_LEN);

    if (new_content[0] != '\0') {
        editWebPage(&b->db, url, new_content);
        mapRemove(&b->cache, url); /* invalidate cache */
        printf(COLOR_GREEN "Konten diperbarui.\n" COLOR_RESET);
    }

    /* Edit linked pages */
    printf("\nMasukkan linked pages baru (Ketik 'DONE' jika selesai, 'SKIP' jika tidak ingin mengubah):\n");
    char linked_url[MAX_URL_LEN];
    printf("  >>> ");
    fflush(stdout);
    if (fgets(linked_url, MAX_URL_LEN, stdin)) {
        int len = (int)strlen(linked_url);
        if (len > 0 && linked_url[len-1] == '\n') linked_url[--len] = '\0';

        if (strcmp(linked_url, "SKIP") != 0) {
            /* Reset linked pages lama */
            removeLinksForPage(&b->ldb, page->id);
            graphRemoveEdgesOf(&b->webGraph, page->id);
            graphAddVertex(&b->webGraph, page->id);

            /* Proses input pertama */
            if (strcmp(linked_url, "DONE") != 0) {
                WebPage *target = NULL;
                if (!isValidUrl(linked_url)) {
                    printf(COLOR_RED "  URL tidak valid!\n" COLOR_RESET);
                } else {
                    target = findWebPageByUrl(&b->db, linked_url);
                    if (!target) {
                        printf(COLOR_RED "  URL tidak ditemukan!\n" COLOR_RESET);
                    } else {
                        addLinkedPage(&b->ldb, page->id, target->id);
                        graphAddEdge(&b->webGraph, page->id, target->id);
                        printf(COLOR_GREEN "  → %s ditambahkan.\n" COLOR_RESET, linked_url);
                    }
                }
                /* Lanjut input berikutnya */
                while (1) {
                    printf("  >>> ");
                    fflush(stdout);
                    if (!fgets(linked_url, MAX_URL_LEN, stdin)) break;
                    len = (int)strlen(linked_url);
                    if (len > 0 && linked_url[len-1] == '\n') linked_url[--len] = '\0';
                    if (strcmp(linked_url, "DONE") == 0) break;
                    if (!isValidUrl(linked_url)) {
                        printf(COLOR_RED "  URL tidak valid!\n" COLOR_RESET);
                        continue;
                    }
                    target = findWebPageByUrl(&b->db, linked_url);
                    if (!target) {
                        printf(COLOR_RED "  URL tidak ditemukan!\n" COLOR_RESET);
                    } else {
                        addLinkedPage(&b->ldb, page->id, target->id);
                        graphAddEdge(&b->webGraph, page->id, target->id);
                        printf(COLOR_GREEN "  → %s ditambahkan.\n" COLOR_RESET, linked_url);
                    }
                }
            }
        }
    }

    printf(COLOR_GREEN "Halaman %s berhasil diperbarui!\n" COLOR_RESET, url);
}

void cmdDeletePage(Browser *b, const char *url) {
    if (!isValidUrl(url)) {
        printf(COLOR_RED "URL tidak valid!\n" COLOR_RESET);
        return;
    }
    WebPage *page = findWebPageByUrl(&b->db, url);
    if (!page) {
        printf(COLOR_RED "Tidak ada halaman dengan URL %s!\n" COLOR_RESET, url);
        return;
    }
    int page_id = page->id;

    /* Cek cache */
    if (mapContains(&b->cache, url)) {
        mapRemove(&b->cache, url);
        printf(COLOR_GREEN "[Status: Cache-Hit]" COLOR_RESET " URL ditemukan di cache dan telah dibersihkan.\n");
    }

    /* Hapus dari linked pages DB */
    printf("Membersihkan relasi linked pages...\n");
    removeLinksForPage(&b->ldb, page_id);

    /* Hapus dari graph */
    graphRemoveVertex(&b->webGraph, page_id);

    /* Hapus dari Set */
    setRemove(&b->urlSet, url);

    /* Hapus dari database */
    deleteWebPage(&b->db, url);

    printf(COLOR_GREEN "Halaman %s berhasil dihapus!\n" COLOR_RESET, url);

    /* Jika halaman yang dihapus adalah yang sedang terbuka, reset state */
    if (b->page_open && strcmp(b->current_url, url) == 0) {
        b->page_open            = 0;
        b->current_url[0]       = '\0';
        b->current_linked_count = 0;
    }
}

/* ================================================================
   F10 - DOWNLOAD MANAGER
   ================================================================ */

void cmdDownload(Browser *b, const char *url) {
    if (!isValidUrl(url)) {
        printf(COLOR_RED "URL tidak valid!\n" COLOR_RESET);
        return;
    }
    if (!urlExists(&b->db, url)) {
        printf(COLOR_RED "404 Not Found! URL tidak ada dalam database.\n" COLOR_RESET);
        return;
    }
    if (queueIsFull(&b->dlQueue)) {
        printf(COLOR_RED "Download tidak diterima, antrian sudah penuh.\n" COLOR_RESET);
        return;
    }
    int ticks = browserCalcTicks(url);
    queueEnqueue(&b->dlQueue, url, ticks);
    int pos = queueSize(&b->dlQueue);
    if (pos == 1) {
        printf(COLOR_GREEN "Download %s (%d ticks)\n" COLOR_RESET, url, ticks);
    } else {
        printf(COLOR_GREEN "Download %s (%d ticks)" COLOR_RESET
               " -> antrian no %d\n", url, ticks, pos);
    }
}

void cmdTick(Browser *b) {
    if (queueIsEmpty(&b->dlQueue)) {
        printf(COLOR_YELLOW "Antrian download saat ini kosong.\n" COLOR_RESET);
        return;
    }
    DownloadItem *front = queueFront(&b->dlQueue);
    front->ticks_remaining--;

    if (front->ticks_remaining <= 0) {
        /* Download selesai */
        printf(COLOR_GREEN "%s selesai terdownload!\n" COLOR_RESET, front->url);
        queueDequeue(&b->dlQueue);
        /* Tampilkan download berikutnya jika ada */
        if (!queueIsEmpty(&b->dlQueue)) {
            DownloadItem *next = queueFront(&b->dlQueue);
            printf("Lanjut downloading %s... (%d ticks tersisa)\n",
                   next->url, next->ticks_remaining);
        }
    } else {
        printf("Downloading %s... (%d ticks tersisa)\n",
               front->url, front->ticks_remaining);
    }
}

/* ================================================================
   F11 - EXIT
   ================================================================ */

void cmdExit(Browser *b) {
    (void)b;
    printf("\n");
    printf(COLOR_CYAN "╔══════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD "         NANGOR FALLS BROWSER         " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╠══════════════════════════════════════╣\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "   Sampai jumpa, Pinus!               " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "   Semoga Deeper segera pulih...      " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "                                      " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_DIM "   \"Sleep is but an illusion\"         " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_DIM "               ~ Crai                 " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╚══════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
}