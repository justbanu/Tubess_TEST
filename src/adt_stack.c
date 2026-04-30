#include <string.h>
#include "../header/adt_stack.h"

void initNavStack(NavStack *ns) {
    ns->current = -1;
    ns->top     = -1;
    for (int i = 0; i < MAX_HISTORY_LEN; i++) {
        ns->urls[i][0] = '\0';
    }
}

/* Push URL baru: hapus semua forward history (dari current+1 ke top),
   lalu tambah di posisi current+1 */
void navPush(NavStack *ns, const char *url) {
    /* Hapus forward history */
    ns->top = ns->current;
    /* Tambah */
    if (ns->top + 1 >= MAX_HISTORY_LEN) {
        /* Geser semua ke kiri agar ada ruang */
        for (int i = 0; i < MAX_HISTORY_LEN - 1; i++) {
            strncpy(ns->urls[i], ns->urls[i+1], MAX_URL_LEN - 1);
        }
        ns->top = MAX_HISTORY_LEN - 2;
        ns->current = ns->top;
    }
    ns->top++;
    ns->current = ns->top;
    strncpy(ns->urls[ns->current], url, MAX_URL_LEN - 1);
    ns->urls[ns->current][MAX_URL_LEN - 1] = '\0';
}

int navBack(NavStack *ns, char *out_url) {
    if (ns->current <= 0) return 0;
    ns->current--;
    strncpy(out_url, ns->urls[ns->current], MAX_URL_LEN - 1);
    out_url[MAX_URL_LEN - 1] = '\0';
    return 1;
}

int navForward(NavStack *ns, char *out_url) {
    if (ns->current >= ns->top) return 0;
    ns->current++;
    strncpy(out_url, ns->urls[ns->current], MAX_URL_LEN - 1);
    out_url[MAX_URL_LEN - 1] = '\0';
    return 1;
}

int navBackN(NavStack *ns, int n, char *out_url) {
    if (ns->current - n < 0) return 0;
    ns->current -= n;
    strncpy(out_url, ns->urls[ns->current], MAX_URL_LEN - 1);
    out_url[MAX_URL_LEN - 1] = '\0';
    return 1;
}

int navForwardN(NavStack *ns, int n, char *out_url) {
    if (ns->current + n > ns->top) return 0;
    ns->current += n;
    strncpy(out_url, ns->urls[ns->current], MAX_URL_LEN - 1);
    out_url[MAX_URL_LEN - 1] = '\0';
    return 1;
}

int navCanBack(NavStack *ns) {
    return ns->current > 0;
}

int navCanForward(NavStack *ns) {
    return ns->current < ns->top;
}

const char* navCurrentUrl(NavStack *ns) {
    if (ns->current < 0) return NULL;
    return ns->urls[ns->current];
}

int navHistoryCount(NavStack *ns) {
    return ns->top + 1;
}

int navGetHistory(NavStack *ns, char out[][MAX_URL_LEN]) {
    int count = ns->top + 1;
    for (int i = 0; i < count; i++) {
        strncpy(out[i], ns->urls[i], MAX_URL_LEN - 1);
        out[i][MAX_URL_LEN - 1] = '\0';
    }
    return count;
}