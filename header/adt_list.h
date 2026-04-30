#ifndef ADT_LIST_H
#define ADT_LIST_H

#include "config.h"

/* ========== ADT LIST (Linked List) ==========
 * Generic linked list untuk keperluan umum,
 * misalnya menyimpan daftar URL hasil search.
 */

typedef struct ListNode {
    char            data[MAX_URL_LEN];
    int             int_data;
    struct ListNode *next;
} ListNode;

typedef struct {
    ListNode *head;
    ListNode *tail;
    int       size;
} LinkedList;

void      listInit(LinkedList *l);
void      listAppend(LinkedList *l, const char *data, int int_data);
void      listPrepend(LinkedList *l, const char *data, int int_data);
ListNode* listGet(LinkedList *l, int index);
void      listClear(LinkedList *l);
int       listSize(LinkedList *l);

#endif /* ADT_LIST_H */