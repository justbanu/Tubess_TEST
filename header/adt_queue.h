#ifndef ADT_QUEUE_H
#define ADT_QUEUE_H

#include "config.h"

/* ========== ADT QUEUE (Circular Array-based) ==========
 * Digunakan untuk Download Manager (F10).
 * Setiap item queue menyimpan URL dan sisa ticks yang diperlukan.
 */

typedef struct {
    char url[MAX_URL_LEN];
    int  ticks_total;
    int  ticks_remaining;
} DownloadItem;

typedef struct {
    DownloadItem items[DOWNLOAD_MAX_AMOUNT];
    int front;
    int rear;
    int size;
    int capacity;
} DownloadQueue;

void          initQueue(DownloadQueue *q, int capacity);
int           queueIsEmpty(DownloadQueue *q);
int           queueIsFull(DownloadQueue *q);
int           queueEnqueue(DownloadQueue *q, const char *url, int ticks);
DownloadItem* queueFront(DownloadQueue *q);
void          queueDequeue(DownloadQueue *q);
int           queueSize(DownloadQueue *q);

#endif /* ADT_QUEUE_H */