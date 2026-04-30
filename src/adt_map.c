#include <string.h>
#include "../header/adt_map.h"

void initMap(Map *m, int capacity) {
    m->size     = 0;
    m->head     = 0;
    m->tail     = 0;
    m->capacity = (capacity > CACHE_MAX_AMOUNT) ? CACHE_MAX_AMOUNT : capacity;
    for (int i = 0; i < CACHE_MAX_AMOUNT; i++) {
        m->entries[i].valid = 0;
        m->entries[i].url[0] = '\0';
    }
}

/* Linear search dalam slot-slot yang valid */
static int mapFind(Map *m, const char *url) {
    for (int i = 0; i < CACHE_MAX_AMOUNT; i++) {
        if (m->entries[i].valid && strcmp(m->entries[i].url, url) == 0) {
            return i;
        }
    }
    return -1;
}

int mapContains(Map *m, const char *url) {
    return mapFind(m, url) != -1;
}

int mapGet(Map *m, const char *url, char *out_content) {
    int idx = mapFind(m, url);
    if (idx == -1) return 0;
    strncpy(out_content, m->entries[idx].content, MAX_CONTENT_LEN - 1);
    out_content[MAX_CONTENT_LEN - 1] = '\0';
    return 1;
}

/* FIFO: evict entry yang pertama masuk (head dalam circular queue) */
int mapPut(Map *m, const char *url, const char *content) {
    /* Jika sudah ada, update saja */
    int idx = mapFind(m, url);
    if (idx != -1) {
        strncpy(m->entries[idx].content, content, MAX_CONTENT_LEN - 1);
        m->entries[idx].content[MAX_CONTENT_LEN - 1] = '\0';
        return 0;
    }

    int evicted = 0;
    if (m->size >= m->capacity) {
        /* Evict: cari slot di circular buffer dari head */
        /* head menunjuk ke slot berikutnya yang akan di-evict */
        int evict_slot = m->head;
        /* pastikan slot ini valid */
        int found = 0;
        for (int i = 0; i < CACHE_MAX_AMOUNT; i++) {
            int slot = (m->head + i) % CACHE_MAX_AMOUNT;
            if (m->entries[slot].valid) {
                evict_slot = slot;
                found = 1;
                break;
            }
        }
        if (found) {
            m->entries[evict_slot].valid = 0;
            m->size--;
            m->head = (evict_slot + 1) % CACHE_MAX_AMOUNT;
        }
        evicted = 1;
    }

    /* Cari slot kosong */
    int insert_slot = m->tail;
    for (int i = 0; i < CACHE_MAX_AMOUNT; i++) {
        int slot = (m->tail + i) % CACHE_MAX_AMOUNT;
        if (!m->entries[slot].valid) {
            insert_slot = slot;
            break;
        }
    }
    strncpy(m->entries[insert_slot].url, url, MAX_URL_LEN - 1);
    m->entries[insert_slot].url[MAX_URL_LEN - 1] = '\0';
    strncpy(m->entries[insert_slot].content, content, MAX_CONTENT_LEN - 1);
    m->entries[insert_slot].content[MAX_CONTENT_LEN - 1] = '\0';
    m->entries[insert_slot].valid = 1;
    m->size++;
    m->tail = (insert_slot + 1) % CACHE_MAX_AMOUNT;

    return evicted;
}

void mapRemove(Map *m, const char *url) {
    int idx = mapFind(m, url);
    if (idx == -1) return;
    m->entries[idx].valid = 0;
    m->size--;
}

void mapClear(Map *m) {
    for (int i = 0; i < CACHE_MAX_AMOUNT; i++) {
        m->entries[i].valid = 0;
    }
    m->size = 0;
    m->head = 0;
    m->tail = 0;
}