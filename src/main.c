#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../header/browser.h"

/* ================================================================
   BANNER & UI
   ================================================================ */

static void printBanner(void) {
    printf(COLOR_CYAN);
    printf("  ________________________________________________________________\n");
    printf(" _  _____ ____    _    _   _ __  __    _    _   _ ___    _    \n");
    printf("| |/ /_ _/ ___|  / \\  | | | |  \\/  |  / \\  | \\ | |_ _|  / \\   \n");
    printf("| ' / | | |     / _ \\ | | | | |\\/| | / _ \\ |  \\| || |  / _ \\  \n");
    printf("| . \\ | | |___ / ___ \\| |_| | |  | |/ ___ \\| |\\  || | / ___ \\ \n");
    printf("|_|\\_\\___\\____/_/   \\_\\\\___/|_|  |_/_/   \\_\\_| \\_|___/_/   \\_\\\n");
    printf(COLOR_RESET);
    printf(COLOR_YELLOW "                  _ KICAU MANIA BROWSER _\n" COLOR_RESET);
    printf(COLOR_DIM    "        BROWSER ANTI PH1SING DAN ANTI PROMPT INJECTON1\n" COLOR_RESET);
    printf("\n");
}

static void printHelp(int in_page) {
    printf(COLOR_CYAN "\n── PERINTAH YANG TERSEDIA ─────────────────────────────────\n" COLOR_RESET);
    if (!in_page) {
        printf("  " COLOR_BOLD "discover" COLOR_RESET "              - Tampilkan 5 URL acak\n");
        printf("  " COLOR_BOLD "search <query>" COLOR_RESET "        - Cari URL berdasarkan prefix\n");
        printf("  " COLOR_BOLD "open <url>" COLOR_RESET "            - Buka halaman web\n");
        printf("  " COLOR_BOLD "add_page <url>" COLOR_RESET "        - Tambah halaman baru\n");
        printf("  " COLOR_BOLD "edit_page <url>" COLOR_RESET "       - Edit halaman\n");
        printf("  " COLOR_BOLD "delete_page <url>" COLOR_RESET "     - Hapus halaman\n");
        printf("  " COLOR_BOLD "newtab" COLOR_RESET "                - Buat tab baru\n");
        printf("  " COLOR_BOLD "closetab" COLOR_RESET "              - Tutup tab aktif\n");
        printf("  " COLOR_BOLD "checktab" COLOR_RESET "              - Lihat daftar tab\n");
        printf("  " COLOR_BOLD "prevtab <n>" COLOR_RESET "           - Pindah n tab ke kiri\n");
        printf("  " COLOR_BOLD "nexttab <n>" COLOR_RESET "           - Pindah n tab ke kanan\n");
        printf("  " COLOR_BOLD "back [n]" COLOR_RESET "              - Kembali n langkah\n");
        printf("  " COLOR_BOLD "forward [n]" COLOR_RESET "           - Maju n langkah\n");
        printf("  " COLOR_BOLD "view_tab_history" COLOR_RESET "      - Lihat history tab aktif\n");
        printf("  " COLOR_BOLD "download <url>" COLOR_RESET "        - Tambah URL ke antrian unduhan\n");
        printf("  " COLOR_BOLD "tick" COLOR_RESET "                  - Majukan waktu download 1 step\n");
        printf("  " COLOR_BOLD "help" COLOR_RESET "                  - Tampilkan bantuan ini\n");
        printf("  " COLOR_BOLD "exit" COLOR_RESET "                  - Keluar dari browser\n");
    } else {
        printf("  " COLOR_BOLD "home" COLOR_RESET "                  - Kembali ke menu utama\n");
        printf("  " COLOR_BOLD "back [n]" COLOR_RESET "              - Kembali n langkah\n");
        printf("  " COLOR_BOLD "forward [n]" COLOR_RESET "           - Maju n langkah\n");
        printf("  " COLOR_BOLD "openlinked <n>" COLOR_RESET "        - Buka linked page ke-n\n");
        printf("  " COLOR_BOLD "view_tab_history" COLOR_RESET "      - Lihat history tab aktif\n");
        printf("  " COLOR_BOLD "download <url>" COLOR_RESET "        - Tambah ke antrian unduhan\n");
        printf("  " COLOR_BOLD "tick" COLOR_RESET "                  - Majukan waktu download\n");
    }
    printf(COLOR_CYAN "───────────────────────────────────────────────────────────\n" COLOR_RESET);
}

/* ================================================================
   COMMAND PARSER
   ================================================================ */

/* Hapus whitespace di awal dan akhir string */
static void trim(char *s) {
    int start = 0;
    while (s[start] && isspace((unsigned char)s[start])) start++;
    if (start > 0) {
        int i = 0;
        while (s[start + i]) { s[i] = s[start + i]; i++; }
        s[i] = '\0';
    }
    int end = (int)strlen(s) - 1;
    while (end >= 0 && isspace((unsigned char)s[end])) { s[end] = '\0'; end--; }
}

void runBrowser(Browser *b) {
    printBanner();
    printf("Ketik " COLOR_BOLD "help" COLOR_RESET " untuk melihat daftar perintah.\n\n");

    char input[512];
    int running = 1;

    while (running) {
        /* Tampilkan prompt */
        Tab *cur_tab = getCurrentTab(&b->tabMgr);
        if (b->page_open) {
            printf(COLOR_BLUE "[%s|%s]" COLOR_RESET " >>> ", cur_tab->name, b->current_url);
        } else {
            printf(COLOR_BLUE "[%s|HOME]" COLOR_RESET " >>> ", cur_tab->name);
        }
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) break;

        /* Hapus newline */
        int inputlen = (int)strlen(input);
        if (inputlen > 0 && input[inputlen-1] == '\n') input[--inputlen] = '\0';
        trim(input);
        if (input[0] == '\0') continue;

        /* Parse command dan argumen */
        char cmd[64]  = {0};
        char arg[450] = {0};
        int i = 0;
        while (input[i] && !isspace((unsigned char)input[i])) {
            if (i < 63) cmd[i] = input[i];
            i++;
        }
        cmd[i < 63 ? i : 63] = '\0';
        while (input[i] && isspace((unsigned char)input[i])) i++;
        strncpy(arg, input + i, sizeof(arg) - 1);
        arg[sizeof(arg) - 1] = '\0';
        trim(arg);

        /* ---- Perintah yang selalu tersedia ---- */
        if (strcmp(cmd, "exit") == 0) {
            cmdExit(b);
            running = 0;

        } else if (strcmp(cmd, "help") == 0) {
            printHelp(b->page_open);

        } else if (strcmp(cmd, "home") == 0) {
            cmdHome(b);

        } else if (strcmp(cmd, "newtab") == 0) {
            cmdNewTab(b);

        } else if (strcmp(cmd, "closetab") == 0) {
            cmdCloseTab(b);

        } else if (strcmp(cmd, "checktab") == 0) {
            cmdCheckTab(b);

        } else if (strcmp(cmd, "prevtab") == 0) {
            if (arg[0] == '\0') {
                printf(COLOR_RED "Usage: prevtab <jarak>\n" COLOR_RESET);
            } else {
                int dist = atoi(arg);
                if (dist <= 0) printf(COLOR_RED "Jarak harus lebih dari 0.\n" COLOR_RESET);
                else cmdPrevTab(b, dist);
            }

        } else if (strcmp(cmd, "nexttab") == 0) {
            if (arg[0] == '\0') {
                printf(COLOR_RED "Usage: nexttab <jarak>\n" COLOR_RESET);
            } else {
                int dist = atoi(arg);
                if (dist <= 0) printf(COLOR_RED "Jarak harus lebih dari 0.\n" COLOR_RESET);
                else cmdNextTab(b, dist);
            }

        } else if (strcmp(cmd, "view_tab_history") == 0) {
            cmdViewTabHistory(b);

        } else if (strcmp(cmd, "tick") == 0) {
            cmdTick(b);

        } else if (strcmp(cmd, "back") == 0) {
            int steps = 1;
            if (arg[0] != '\0') {
                steps = atoi(arg);
                if (steps <= 0) { printf(COLOR_RED "Langkah harus lebih dari 0.\n" COLOR_RESET); continue; }
            }
            cmdBack(b, steps);

        } else if (strcmp(cmd, "forward") == 0) {
            int steps = 1;
            if (arg[0] != '\0') {
                steps = atoi(arg);
                if (steps <= 0) { printf(COLOR_RED "Langkah harus lebih dari 0.\n" COLOR_RESET); continue; }
            }
            cmdForward(b, steps);

        /* ---- Perintah di halaman web ---- */
        } else if (strcmp(cmd, "openlinked") == 0) {
            if (arg[0] == '\0') {
                printf(COLOR_RED "Usage: openlinked <nomor>\n" COLOR_RESET);
            } else {
                int idx = atoi(arg);
                cmdOpenLinked(b, idx);
            }

        /* ---- Perintah di menu utama ---- */
        } else if (strcmp(cmd, "discover") == 0) {
            cmdDiscover(b);

        } else if (strcmp(cmd, "search") == 0) {
            if (arg[0] == '\0') {
                printf(COLOR_RED "Usage: search <query>\n" COLOR_RESET);
            } else {
                /* Ambil hanya 1 kata sebagai query jika ada spasi */
                char query[MAX_URL_LEN];
                strncpy(query, arg, MAX_URL_LEN - 1);
                query[MAX_URL_LEN - 1] = '\0';
                /* Potong di spasi pertama */
                for (int qi = 0; query[qi]; qi++) {
                    if (isspace((unsigned char)query[qi])) { query[qi] = '\0'; break; }
                }
                cmdSearch(b, query);
            }

        } else if (strcmp(cmd, "open") == 0) {
            if (arg[0] == '\0') {
                printf(COLOR_RED "Usage: open <url>\n" COLOR_RESET);
            } else {
                cmdOpen(b, arg);
            }

        } else if (strcmp(cmd, "add_page") == 0) {
            if (arg[0] == '\0') {
                printf(COLOR_RED "Usage: add_page <url>\n" COLOR_RESET);
            } else {
                if (b->page_open) {
                    printf(COLOR_YELLOW "Peringatan: Tutup halaman dulu (home) sebelum mengelola database.\n" COLOR_RESET);
                }
                cmdAddPage(b, arg);
            }

        } else if (strcmp(cmd, "edit_page") == 0) {
            if (arg[0] == '\0') {
                printf(COLOR_RED "Usage: edit_page <url>\n" COLOR_RESET);
            } else {
                cmdEditPage(b, arg);
            }

        } else if (strcmp(cmd, "delete_page") == 0) {
            if (arg[0] == '\0') {
                printf(COLOR_RED "Usage: delete_page <url>\n" COLOR_RESET);
            } else {
                cmdDeletePage(b, arg);
            }

        } else if (strcmp(cmd, "download") == 0) {
            if (arg[0] == '\0') {
                printf(COLOR_RED "Usage: download <url>\n" COLOR_RESET);
            } else {
                cmdDownload(b, arg);
            }

        } else {
            printf(COLOR_RED "Perintah tidak dikenal: '%s'. Ketik 'help' untuk bantuan.\n" COLOR_RESET, cmd);
        }
    }
}

/* ================================================================
   MAIN
   ================================================================ */

int main(void) {
    Browser browser;
    browserInit(&browser);
    runBrowser(&browser);
    setDestroy(&browser.urlSet);
    return 0;
}