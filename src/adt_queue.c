#include <string.h>
#include "../header/adt_queue.h"

void initQueue(DownloadQueue *q, int capacity) {
    q->front    = 0;
    q->rear     = 0;
    q->size     = 0;
    q->capacity = (capacity > DOWNLOAD_MAX_AMOUNT) ? DOWNLOAD_MAX_AMOUNT : capacity;
}

int queueIsEmpty(DownloadQueue *q) {
    return q->size == 0;
}

int queueIsFull(DownloadQueue *q) {
    return q->size >= q->capacity;
}

int queueEnqueue(DownloadQueue *q, const char *url, int ticks) {
    if (queueIsFull(q)) return 0;
    strncpy(q->items[q->rear].url, url, MAX_URL_LEN - 1);
    q->items[q->rear].url[MAX_URL_LEN - 1] = '\0';
    q->items[q->rear].ticks_total     = ticks;
    q->items[q->rear].ticks_remaining = ticks;
    q->rear = (q->rear + 1) % DOWNLOAD_MAX_AMOUNT;
    q->size++;
    return 1;
}

DownloadItem* queueFront(DownloadQueue *q) {
    if (queueIsEmpty(q)) return NULL;
    return &q->items[q->front];
}

void queueDequeue(DownloadQueue *q) {
    if (queueIsEmpty(q)) return;
    q->front = (q->front + 1) % DOWNLOAD_MAX_AMOUNT;
    q->size--;
}

int queueSize(DownloadQueue *q) {
    return q->size;
}