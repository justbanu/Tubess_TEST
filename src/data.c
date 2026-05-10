#include <stdio.h>
#include "../header/data.h"

/* ================================================================
   HELPER STATIS
   ================================================================ */

static void addPage(WebDatabase *db, Set *urlSet, Graph *webGraph,
                    int id, const char *url, const char *content) {
    db->next_id = id;
    int real_id = addWebPage(db, url, content);
    if (real_id < 0) real_id = id;
    setInsert(urlSet, url, real_id);
    graphAddVertex(webGraph, real_id);
}

static void addLink(WebDatabase *db, LinkedPagesDB *ldb, Graph *webGraph,
                    const char *src_url, const char *dst_url) {
    WebPage *p = findWebPageByUrl(db, src_url);
    WebPage *q = findWebPageByUrl(db, dst_url);
    if (p && q) {
        addLinkedPage(ldb, p->id, q->id);
        graphAddEdge(webGraph, p->id, q->id);
    }
}

/* ================================================================
   HALAMAN WEB (HARD-CODED)
   ================================================================ */

static void initPages(WebDatabase *db, Set *urlSet, Graph *webGraph) {
    addPage(db, urlSet, webGraph, 1, "mysteryshack.com",
        "SELAMAT DATANG DI MYSTERY SHACK!\n"
        "Tempat paling ajaib di Nangor Falls (dan satu-satunya yang punya diskon 0%!).\n"
        "Lihatlah Jackalope asli! Lihatlah batu yang menyerupai wajah Stun!\n"
        "Peringatan: Barang yang sudah dibeli tidak dapat dikembalikan,\n"
        "apalagi kalau Anda baru sadar itu cuma barang rongsokan yang ditempel lem.");

    addPage(db, urlSet, webGraph, 2, "gideon-tent.org",
        "BACA MASA DEPANMU DI SINI!\n"
        "\"Lil' Gideon\" tahu segalanya tentangmu.\n"
        "Bahkan rahasia yang kau sembunyikan di bawah tempat tidur.\n"
        "Datanglah ke 'Tent of Telepathy', sekarang juga!");

    addPage(db, urlSet, webGraph, 3, "journal-3.nf",
        "JURNAL NO. 3 - CLASSIFIED\n"
        "Ditulis oleh seorang penulis misterius yang tahu tentang semua anomali Nangor Falls.\n"
        "Jangan sampai buku ini jatuh ke tangan yang salah.\n"
        "Perhatian: Konten jurnal ini sangat rahasia.");

    addPage(db, urlSet, webGraph, 4, "batu-ajaib-asli.com",
        "BATU AJAIB ASLI - LANGSUNG DARI GUA MISTERIUS!\n"
        "Batu ini terbukti secara ilmiah memiliki kekuatan supranatural.\n"
        "(Bukti ilmiahnya sendiri masih dalam penelitian.)\n"
        "Dapatkan sekarang dengan harga spesial: Rp 999.999!");

    addPage(db, urlSet, webGraph, 5, "topi-pinus-original.com",
        "TOPI PINUS IKONIK - SEPERTI YANG DIPAKAI ANAK ITU!\n"
        "Stok terbatas! Dapatkan topi biru-putih dengan logo pohon pinus yang melegenda.\n"
        "Dijamin 100% serat kain asli (mungkin).");

    addPage(db, urlSet, webGraph, 6, "topi-pinus-kw.com",
        "TOPI PINUS KW SUPER - TIDAK KALAH KEREN!\n"
        "Harga lebih terjangkau, kualitas juga... cukup.\n"
        "Tersedia dalam berbagai ukuran, warna terbatas: biru dan biru tua.");

    addPage(db, urlSet, webGraph, 7, "nangor-falls.info",
        "SELAMAT DATANG DI NANGOR FALLS!\n"
        "Kota kecil yang penuh dengan misteri dan keajaiban.\n"
        "Populasi: sekitar 3,000 jiwa (dan beberapa entitas non-manusia).\n"
        "Objek wisata: Mystery Shack, Tent of Telepathy, Danau Danau.");

    addPage(db, urlSet, webGraph, 8, "cipher.nf",
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

    addPage(db, urlSet, webGraph, 9, "six.itb.ac.id",
        "SIX - Sistem Informasi Akademik ITB\n"
        "Selamat datang di portal akademik Institut Teknologi Bandung.\n"
        "Silakan login untuk mengakses nilai, jadwal, dan informasi akademik lainnya.\n"
        "Hotline: 022-1234-5678");

    addPage(db, urlSet, webGraph, 10, "mystery-journals.net",
        "MYSTERY JOURNALS NETWORK\n"
        "Kumpulan jurnal-jurnal misterius dari seluruh dunia.\n"
        "Bergabunglah dengan komunitas peneliti paranormal terbesar di internet.\n"
        "Member aktif: 42,000+ orang (dan beberapa entitas).");
}

/* ================================================================
   RELASI LINKED PAGES (HARD-CODED)
   ================================================================ */

static void populateGraph(WebDatabase *db, LinkedPagesDB *ldb, Graph *webGraph) {
    /* mysteryshack.com -> batu-ajaib-asli.com, topi-pinus-original.com */
    addLink(db, ldb, webGraph, "mysteryshack.com", "batu-ajaib-asli.com");
    addLink(db, ldb, webGraph, "mysteryshack.com", "topi-pinus-original.com");

    /* gideon-tent.org -> mysteryshack.com, mystery-journals.net */
    addLink(db, ldb, webGraph, "gideon-tent.org", "mysteryshack.com");
    addLink(db, ldb, webGraph, "gideon-tent.org", "mystery-journals.net");

    /* journal-3.nf -> cipher.nf, nangor-falls.info */
    addLink(db, ldb, webGraph, "journal-3.nf", "cipher.nf");
    addLink(db, ldb, webGraph, "journal-3.nf", "nangor-falls.info");

    /* nangor-falls.info -> mysteryshack.com, gideon-tent.org */
    addLink(db, ldb, webGraph, "nangor-falls.info", "mysteryshack.com");
    addLink(db, ldb, webGraph, "nangor-falls.info", "gideon-tent.org");

    /* topi-pinus-original.com -> topi-pinus-kw.com */
    addLink(db, ldb, webGraph, "topi-pinus-original.com", "topi-pinus-kw.com");

    /* six.itb.ac.id -> nangor-falls.info */
    addLink(db, ldb, webGraph, "six.itb.ac.id", "nangor-falls.info");
}

/* ================================================================
   ENTRY POINT
   ================================================================ */

void initData(WebDatabase *db, LinkedPagesDB *ldb, Set *urlSet, Graph *webGraph) {
    initPages(db, urlSet, webGraph);
    populateGraph(db, ldb, webGraph);
    printf(COLOR_GREEN "[INIT]" COLOR_RESET " Hard-coded data initialized\n\n");
}
