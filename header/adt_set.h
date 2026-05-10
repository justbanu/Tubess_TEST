#ifndef ADT_SET_H
#define ADT_SET_H

#include "config.h"

/* ========== ADT SET (Linked List-based) ==========
 * Menyimpan kumpulan URL unik, digunakan sebagai daftar utama halaman web.
 * Linear search dilakukan di sini (F03).
 */

typedef struct SetNode {
    char           url[MAX_URL_LEN];
    int            page_id;
    struct SetNode *next;
} SetNode;

typedef struct {
    SetNode *head;
    int      size;
} Set;

void  initSet(Set *s);
int   setContains(Set *s, const char *url);
int   setInsert(Set *s, const char *url, int page_id);   /* return 1 sukses, 0 duplikat */
void  setRemove(Set *s, const char *url);
int   setGetPageId(Set *s, const char *url);             /* return -1 jika tidak ada */
void  setDestroy(Set *s);

#endif /* ADT_SET_H */