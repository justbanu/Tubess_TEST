#include <stdlib.h>
#include <string.h>
#include "../header/adt_graph.h"

void initGraph(Graph *g) {
    g->vertex_count = 0;
    g->edge_count   = 0;
    for (int i = 0; i < MAX_WEB_PAGES; i++) {
        g->vertices[i].page_id = -1;
        g->vertices[i].active  = 0;
        g->vertices[i].edges   = NULL;
    }
}

int graphFindVertex(Graph *g, int page_id) {
    for (int i = 0; i < g->vertex_count; i++) {
        if (g->vertices[i].active && g->vertices[i].page_id == page_id) {
            return i;
        }
    }
    return -1;
}

int graphHasVertex(Graph *g, int page_id) {
    return graphFindVertex(g, page_id) != -1;
}

int graphAddVertex(Graph *g, int page_id) {
    if (graphHasVertex(g, page_id)) return 0;
    /* Cari slot kosong */
    for (int i = 0; i < MAX_WEB_PAGES; i++) {
        if (!g->vertices[i].active) {
            g->vertices[i].page_id = page_id;
            g->vertices[i].active  = 1;
            g->vertices[i].edges   = NULL;
            if (i >= g->vertex_count) g->vertex_count = i + 1;
            return 1;
        }
    }
    return 0;
}

int graphRemoveVertex(Graph *g, int page_id) {
    int idx = graphFindVertex(g, page_id);
    if (idx == -1) return 0;
    /* Hapus semua edge keluar */
    GraphEdge *e = g->vertices[idx].edges;
    while (e) {
        GraphEdge *next = e->next;
        free(e);
        e = next;
    }
    g->vertices[idx].edges  = NULL;
    g->vertices[idx].active = 0;
    /* Hapus edge masuk dari vertex lain */
    for (int i = 0; i < g->vertex_count; i++) {
        if (!g->vertices[i].active) continue;
        GraphEdge *prev = NULL;
        GraphEdge *cur  = g->vertices[i].edges;
        while (cur) {
            if (cur->dest_id == page_id) {
                if (prev) prev->next = cur->next;
                else      g->vertices[i].edges = cur->next;
                free(cur);
                g->edge_count--;
                cur = prev ? prev->next : g->vertices[i].edges;
            } else {
                prev = cur;
                cur  = cur->next;
            }
        }
    }
    return 1;
}

void graphRemoveEdgesOf(Graph *g, int page_id) {
    graphRemoveVertex(g, page_id);
}

int graphAddEdge(Graph *g, int src_id, int dest_id) {
    /* Pastikan kedua vertex ada */
    if (!graphHasVertex(g, src_id))  graphAddVertex(g, src_id);
    if (!graphHasVertex(g, dest_id)) graphAddVertex(g, dest_id);

    int src_idx = graphFindVertex(g, src_id);
    if (src_idx == -1) return 0;

    /* Cek duplikat */
    GraphEdge *e = g->vertices[src_idx].edges;
    while (e) {
        if (e->dest_id == dest_id) return 0; /* sudah ada */
        e = e->next;
    }

    /* Tambahkan edge */
    GraphEdge *newEdge = (GraphEdge*)malloc(sizeof(GraphEdge));
    if (!newEdge) return 0;
    newEdge->dest_id = dest_id;
    newEdge->next    = g->vertices[src_idx].edges;
    g->vertices[src_idx].edges = newEdge;
    g->edge_count++;
    return 1;
}

int graphGetNeighbors(Graph *g, int src_id, int *out_ids) {
    int src_idx = graphFindVertex(g, src_id);
    if (src_idx == -1) return 0;
    int count = 0;
    GraphEdge *e = g->vertices[src_idx].edges;
    while (e) {
        out_ids[count++] = e->dest_id;
        e = e->next;
    }
    return count;
}