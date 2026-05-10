#ifndef ADT_STACK_H
#define ADT_STACK_H

#include "config.h"

/* ========== ADT STACK (Array-based) ==========
 * Digunakan untuk back/forward history pada setiap Tab.
 * Stack menyimpan URL yang pernah dikunjungi.
 * Pointer 'current' menunjuk posisi halaman aktif.
 */

typedef struct {
    char urls[MAX_HISTORY_LEN][MAX_URL_LEN];
    int  current;   /* indeks halaman aktif (-1 jika kosong) */
    int  top;       /* indeks top of stack (terakhir yang pernah ada) */
} NavStack;

void        initNavStack(NavStack *ns);
void        navPush(NavStack *ns, const char *url);  /* push sekaligus hapus forward history */
int         navBack(NavStack *ns, char *out_url);    /* return 1 jika berhasil */
int         navForward(NavStack *ns, char *out_url); /* return 1 jika berhasil */
int         navBackN(NavStack *ns, int n, char *out_url);
int         navForwardN(NavStack *ns, int n, char *out_url);
int         navCanBack(NavStack *ns);
int         navCanForward(NavStack *ns);
const char* navCurrentUrl(NavStack *ns);             /* NULL jika kosong */
int         navHistoryCount(NavStack *ns);
/* Isi out array dengan semua URL di history (indeks 0..top), return count */
int         navGetHistory(NavStack *ns, char out[][MAX_URL_LEN]);

#endif /* ADT_STACK_H */