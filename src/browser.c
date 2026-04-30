#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../header/browser.h"
#include "../header/csv_parser.h"

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

    b->current_url[0]      = '\0';
    b->page_open           = 0;
    b->current_linked_count = 0;
}

/* Menambahkan halaman ke semua struktur data */
static void browserAddPageInternal(Browser *b, int id, const char *url, const char *content) {
    int real_id = addWebPage(&b->db, url, content);
    if (real_id < 0) real_id = id; /* fallback jika gagal */
    setInsert(&b->urlSet, url, real_id);
    graphAddVertex(&b->webGraph, real_id);
}

/* Menambahkan relasi link ke semua struktur data */
static void browserAddLinkInternal(Browser *b, int src_id, int dst_id) {
    addLinkedPage(&b->ldb, src_id, dst_id);
    graphAddEdge(&b->webGraph, src_id, dst_id);
}

/* ================================================================
   DUMMY DATA (untuk STI: data hardcoded)
   ================================================================ */

void browserLoadDummyData(Browser *b) {
    /* Halaman web */
    browserAddPageInternal(b, 1, "mysteryshack.com",
        "SELAMAT DATANG DI MYSTERY SHACK!\n"
        "Tempat paling ajaib di Nangor Falls (dan satu-satunya yang punya diskon 0%!).\n"
        "Lihatlah Jackalope asli! Lihatlah batu yang menyerupai wajah Stun!\n"
        "Peringatan: Barang yang sudah dibeli tidak dapat dikembalikan,\n"
        "apalagi kalau Anda baru sadar itu cuma barang rongsokan yang ditempel lem.");

    browserAddPageInternal(b, 2, "gideon-tent.org",
        "BACA MASA DEPANMU DI SINI!\n"
        "\"Lil' Gideon\" tahu segalanya tentangmu.\n"
        "Bahkan rahasia yang kau sembunyikan di bawah tempat tidur.\n"
        "Datanglah ke 'Tent of Telepathy', sekarang juga!");

    browserAddPageInternal(b, 3, "journal-3.nf",
        "JURNAL NO. 3 - CLASSIFIED\n"
        "Ditulis oleh seorang penulis misterius yang tahu tentang semua anomali Nangor Falls.\n"
        "Jangan sampai buku ini jatuh ke tangan yang salah.\n"
        "Perhatian: Konten jurnal ini sangat rahasia.");

    browserAddPageInternal(b, 4, "batu-ajaib-asli.com",
        "BATU AJAIB ASLI - LANGSUNG DARI GUA MISTERIUS!\n"
        "Batu ini terbukti secara ilmiah memiliki kekuatan supranatural.\n"
        "(Bukti ilmiahnya sendiri masih dalam penelitian.)\n"
        "Dapatkan sekarang dengan harga spesial: Rp 999.999!");

    browserAddPageInternal(b, 5, "topi-pinus-original.com",
        "TOPI PINUS IKONIK - SEPERTI YANG DIPAKAI ANAK ITU!\n"
        "Stok terbatas! Dapatkan topi biru-putih dengan logo pohon pinus yang melegenda.\n"
        "Dijamin 100% serat kain asli (mungkin).");

    browserAddPageInternal(b, 6, "topi-pinus-kw.com",
        "TOPI PINUS KW SUPER - TIDAK KALAH KEREN!\n"
        "Harga lebih terjangkau, kualitas juga... cukup.\n"
        "Tersedia dalam berbagai ukuran, warna terbatas: biru dan biru tua.");

    browserAddPageInternal(b, 7, "nangor-falls.info",
        "SELAMAT DATANG DI NANGOR FALLS!\n"
        "Kota kecil yang penuh dengan misteri dan keajaiban.\n"
        "Populasi: sekitar 3,000 jiwa (dan beberapa entitas non-manusia).\n"
        "Objek wisata: Mystery Shack, Tent of Telepathy, Danau Danau.");

    browserAddPageInternal(b, 8, "cipher.nf",
        " $$$$$$\\  $$$$$$\\ $$$$$$$\\  $$\\   $$\\ $$$$$$$$\\ $$$$$$$\\ \n"
        "$$  __$$\\ \\_$$  _|$$  __$$\\ $$ |  $$ |$$  _____|$$  __$$\\\n"
        "$$ /  \\__| $$ |  $$ |  $$ |$$ |  $$ |$$ |      $$ |  $$ |\n"
        "$$ |       $$ |  $$$$$$$ |$$$$$$$$ |$$$$\\    $$$$$$$ |\n"
        "$$ |       $$ |  $$  ____/ $$  __$$ |$$  __|   $$  __$$< \n"
        "$$ |  $$\\ $$ |  $$ |      $$ |  $$ |$$ |      $$ |  $$ |\n"
        "\\$$$$$$  |$$$$$$\\ $$ |      $$ |  $$ |$$$$$$$$\\ $$ |  $$ |\n"
        " \\______/ \\______|\\__|      \\__|  \\__|\\________|\\__|  \\__|\n"
        "\n"
        "You thought you could stop me? How adorable.\n"
        "I've been watching. I've always been watching.");

    browserAddPageInternal(b, 9, "six.itb.ac.id",
        "SIX - Sistem Informasi Akademik ITB\n"
        "Selamat datang di portal akademik Institut Teknologi Bandung.\n"
        "Silakan login untuk mengakses nilai, jadwal, dan informasi akademik lainnya.\n"
        "Hotline: 022-1234-5678");

    browserAddPageInternal(b, 10, "mystery-journals.net",
        "MYSTERY JOURNALS NETWORK\n"
        "Kumpulan jurnal-jurnal misterius dari seluruh dunia.\n"
        "Bergabunglah dengan komunitas peneliti paranormal terbesar di internet.\n"
        "Member aktif: 42,000+ orang (dan beberapa entitas).");

    /* Relasi linked pages (id dari pages di atas: 1..10) */
    /* mysteryshack.com -> batu-ajaib-asli.com, topi-pinus-original.com */
    WebPage *p;
    p = findWebPageByUrl(&b->db, "mysteryshack.com");
    WebPage *q;
    q = findWebPageByUrl(&b->db, "batu-ajaib-asli.com");
    if (p && q) browserAddLinkInternal(b, p->id, q->id);
    q = findWebPageByUrl(&b->db, "topi-pinus-original.com");
    if (p && q) browserAddLinkInternal(b, p->id, q->id);

    /* gideon-tent.org -> mysteryshack.com, mystery-journals.net */
    p = findWebPageByUrl(&b->db, "gideon-tent.org");
    q = findWebPageByUrl(&b->db, "mysteryshack.com");
    if (p && q) browserAddLinkInternal(b, p->id, q->id);
    q = findWebPageByUrl(&b->db, "mystery-journals.net");
    if (p && q) browserAddLinkInternal(b, p->id, q->id);

    /* journal-3.nf -> cipher.nf, nangor-falls.info */
    p = findWebPageByUrl(&b->db, "journal-3.nf");
    q = findWebPageByUrl(&b->db, "cipher.nf");
    if (p && q) browserAddLinkInternal(b, p->id, q->id);
    q = findWebPageByUrl(&b->db, "nangor-falls.info");
    if (p && q) browserAddLinkInternal(b, p->id, q->id);

    /* nangor-falls.info -> mysteryshack.com, gideon-tent.org */
    p = findWebPageByUrl(&b->db, "nangor-falls.info");
    q = findWebPageByUrl(&b->db, "mysteryshack.com");
    if (p && q) browserAddLinkInternal(b, p->id, q->id);
    q = findWebPageByUrl(&b->db, "gideon-tent.org");
    if (p && q) browserAddLinkInternal(b, p->id, q->id);

    /* topi-pinus-original.com -> topi-pinus-kw.com */
    p = findWebPageByUrl(&b->db, "topi-pinus-original.com");
    q = findWebPageByUrl(&b->db, "topi-pinus-kw.com");
    if (p && q) browserAddLinkInternal(b, p->id, q->id);

    /* six.itb.ac.id -> nangor-falls.info */
    p = findWebPageByUrl(&b->db, "six.itb.ac.id");
    q = findWebPageByUrl(&b->db, "nangor-falls.info");
    if (p && q) browserAddLinkInternal(b, p->id, q->id);
}

/* ================================================================
   HELPER: TAMPILKAN HALAMAN
   ================================================================ */

int browserCalcTicks(const char *url) {
    int len = (int)strlen(url);
    return len / 5 + 2;
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

/* ================================================================
   REBUILD GRAPH & URLSET DARI DATA YANG SUDAH DILOAD
   ================================================================ */

/* Bangun ulang urlSet dari database (dipakai setelah load CSV) */
void browserRebuildUrlSet(Browser *b) {
    setDestroy(&b->urlSet);
    initSet(&b->urlSet);
    for (int i = 0; i < b->db.total; i++) {
        if (b->db.pages[i].active) {
            setInsert(&b->urlSet, b->db.pages[i].url, b->db.pages[i].id);
        }
    }
}

/* Bangun ulang ADT Graph dari LinkedPagesDB (dipakai setelah load CSV) */
void browserRebuildGraph(Browser *b) {
    initGraph(&b->webGraph);
    /* Tambahkan semua vertex */
    for (int i = 0; i < b->db.total; i++) {
        if (b->db.pages[i].active) {
            graphAddVertex(&b->webGraph, b->db.pages[i].id);
        }
    }
    /* Tambahkan semua edge dari ldb */
    for (int i = 0; i < MAX_WEB_PAGES * 5; i++) {
        if (b->ldb.relations[i].active) {
            graphAddEdge(&b->webGraph,
                         b->ldb.relations[i].id_sumber,
                         b->ldb.relations[i].id_tujuan);
        }
    }
}

/* ================================================================
   LOAD DARI CSV
   ================================================================ */

int browserLoadFromCSV(Browser *b, const char *config_folder) {
    /* Bangun path lengkap */
    char path_pages[512];
    char path_links[512];

    /* Pastikan folder diakhiri dengan '/' */
    int flen = (int)strlen(config_folder);
    if (flen > 0 && config_folder[flen - 1] == '/') {
        snprintf(path_pages, sizeof(path_pages), "%sweb_pages.csv",    config_folder);
        snprintf(path_links, sizeof(path_links), "%slinked_pages.csv", config_folder);
    } else {
        snprintf(path_pages, sizeof(path_pages), "%s/web_pages.csv",    config_folder);
        snprintf(path_links, sizeof(path_links), "%s/linked_pages.csv", config_folder);
    }

    printf(COLOR_CYAN "[LOAD]" COLOR_RESET " Memuat data dari folder: %s\n", config_folder);

    /* Reset semua data */
    initWebDatabase(&b->db);
    initLinkedPagesDB(&b->ldb);
    setDestroy(&b->urlSet);
    initSet(&b->urlSet);
    mapClear(&b->cache);
    initGraph(&b->webGraph);

    /* Load web_pages.csv */
    int pages_loaded = csvLoadWebPages(path_pages, &b->db);
    if (pages_loaded < 0) {
        printf(COLOR_RED "[LOAD ERROR]" COLOR_RESET
               " File %s tidak ditemukan!\n", path_pages);
        printf(COLOR_YELLOW "         Menggunakan data bawaan (dummy data)...\n" COLOR_RESET);
        browserLoadDummyData(b);
        return 0;
    }
    printf(COLOR_GREEN "  ✓" COLOR_RESET " web_pages.csv     : %d halaman dimuat\n", pages_loaded);

    /* Load linked_pages.csv */
    int links_loaded = csvLoadLinkedPages(path_links, &b->ldb);
    if (links_loaded < 0) {
        printf(COLOR_YELLOW "  ⚠" COLOR_RESET
               " linked_pages.csv tidak ditemukan (tidak ada linked pages).\n");
        links_loaded = 0;
    } else {
        printf(COLOR_GREEN "  ✓" COLOR_RESET " linked_pages.csv  : %d relasi dimuat\n", links_loaded);
    }

    /* Rebuild urlSet dan Graph dari data yang baru diload */
    browserRebuildUrlSet(b);
    browserRebuildGraph(b);

    printf(COLOR_GREEN "[LOAD SELESAI]" COLOR_RESET
           " %d halaman, %d relasi berhasil dimuat.\n\n",
           pages_loaded, links_loaded);
    return 1;
}