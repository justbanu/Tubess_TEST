#include <stdlib.h>
#include <string.h>
#include "../header/adt_list.h"

void listInit(LinkedList *l) {
    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
}

void listAppend(LinkedList *l, const char *data, int int_data) {
    ListNode *node = (ListNode*)malloc(sizeof(ListNode));
    if (!node) return;
    strncpy(node->data, data, MAX_URL_LEN - 1);
    node->data[MAX_URL_LEN - 1] = '\0';
    node->int_data = int_data;
    node->next = NULL;
    if (l->tail) {
        l->tail->next = node;
    } else {
        l->head = node;
    }
    l->tail = node;
    l->size++;
}

void listPrepend(LinkedList *l, const char *data, int int_data) {
    ListNode *node = (ListNode*)malloc(sizeof(ListNode));
    if (!node) return;
    strncpy(node->data, data, MAX_URL_LEN - 1);
    node->data[MAX_URL_LEN - 1] = '\0';
    node->int_data = int_data;
    node->next = l->head;
    l->head = node;
    if (!l->tail) l->tail = node;
    l->size++;
}

ListNode* listGet(LinkedList *l, int index) {
    if (index < 0 || index >= l->size) return NULL;
    ListNode *cur = l->head;
    for (int i = 0; i < index; i++) cur = cur->next;
    return cur;
}

void listClear(LinkedList *l) {
    ListNode *cur = l->head;
    while (cur) {
        ListNode *next = cur->next;
        free(cur);
        cur = next;
    }
    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
}

int listSize(LinkedList *l) {
    return l->size;
}