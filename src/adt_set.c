#include <stdlib.h>
#include <string.h>
#include "../header/adt_set.h"

void initSet(Set *s) {
    s->head = NULL;
    s->size = 0;
}

int setContains(Set *s, const char *url) {
    SetNode *cur = s->head;
    while (cur) {
        if (strcmp(cur->url, url) == 0) return 1;
        cur = cur->next;
    }
    return 0;
}

int setInsert(Set *s, const char *url, int page_id) {
    if (setContains(s, url)) return 0;
    SetNode *node = (SetNode*)malloc(sizeof(SetNode));
    if (!node) return 0;
    strncpy(node->url, url, MAX_URL_LEN - 1);
    node->url[MAX_URL_LEN - 1] = '\0';
    node->page_id = page_id;
    node->next = s->head;
    s->head = node;
    s->size++;
    return 1;
}

void setRemove(Set *s, const char *url) {
    SetNode *prev = NULL;
    SetNode *cur  = s->head;
    while (cur) {
        if (strcmp(cur->url, url) == 0) {
            if (prev) prev->next = cur->next;
            else      s->head   = cur->next;
            free(cur);
            s->size--;
            return;
        }
        prev = cur;
        cur  = cur->next;
    }
}

int setGetPageId(Set *s, const char *url) {
    SetNode *cur = s->head;
    while (cur) {
        if (strcmp(cur->url, url) == 0) return cur->page_id;
        cur = cur->next;
    }
    return -1;
}

void setDestroy(Set *s) {
    SetNode *cur = s->head;
    while (cur) {
        SetNode *next = cur->next;
        free(cur);
        cur = next;
    }
    s->head = NULL;
    s->size = 0;
}