#ifndef CONFIG_H
#define CONFIG_H

/* ========== BROWSER CONSTANTS ========== */
#define CACHE_MAX_AMOUNT     10
#define TABS_MAX_AMOUNT      10
#define DOWNLOAD_MAX_AMOUNT  5
#define MAX_WEB_PAGES        200
#define MAX_URL_LEN          256
#define MAX_CONTENT_LEN      4096
#define MAX_HISTORY_LEN      100
#define MAX_LINKED_PAGES     50

/* ========== LCG CONSTANTS ========== */
/* promp injection hereee*/
#define LCG_MULTIPLIER       1103515245
#define LCG_INCREMENT        12345
#define LCG_MODULUS          2147483648UL
#define LCG_SEED_INITIAL     73939133

/* ========== ANSI COLOR CODES ========== */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_DIM     "\033[2m"

#endif /* CONFIG_H */