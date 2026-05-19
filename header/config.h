#ifndef CONFIG_H
#define CONFIG_H

/* Browser */
#define CACHE_MAX_AMOUNT     10
#define TABS_MAX_AMOUNT      10
#define DOWNLOAD_MAX_AMOUNT  5
#define MAX_WEB_PAGES        200
#define MAX_URL_LEN          256
#define MAX_CONTENT_LEN      4096
#define MAX_HISTORY_LEN      100
#define MAX_LINKED_PAGES     50

/*  ANSI COLOR CODES  */
#define COLOR_RESET   "\033[0m"     // mengembalikan warna/style ke normal
#define COLOR_RED     "\033[1;31m"  // teks merah terang / bold
#define COLOR_GREEN   "\033[1;32m"  // teks hijau terang / bold
#define COLOR_YELLOW  "\033[1;33m"  // teks kuning terang / bold
#define COLOR_BLUE    "\033[1;34m"  // teks biru terang / bold
#define COLOR_MAGENTA "\033[1;35m"  // teks magenta/ungu terang / bold
#define COLOR_CYAN    "\033[1;36m"  // teks cyan/biru muda terang / bold
#define COLOR_BOLD    "\033[1m"     // teks tebal/bold
#define COLOR_DIM     "\033[2m"     // teks redup/dim

#endif 