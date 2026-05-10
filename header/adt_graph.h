#ifndef ADT_GRAPH_H
#define ADT_GRAPH_H

#include "config.h"

/* ========== ADT GRAPH (Adjacency List, Directed) ==========
 * Digunakan untuk Web Graph (F09) - spesifikasi STI.
 * Node = halaman web (diidentifikasi oleh URL).
 * Edge = linked page (dari sumber ke tujuan).
 */

/* Node dalam adjacency list */
typedef struct GraphEdge {
    int              dest_id;       /* page_id tujuan */
    struct GraphEdge *next;
} GraphEdge;

/* Vertex graph */
typedef struct {
    int       page_id;
    int       active;
    GraphEdge *edges;               /* linked list edge keluar */
} GraphVertex;

typedef struct {
    GraphVertex vertices[MAX_WEB_PAGES];
    int         vertex_count;
    int         edge_count;
} Graph;

void  initGraph(Graph *g);
int   graphAddVertex(Graph *g, int page_id);
int   graphRemoveVertex(Graph *g, int page_id);
int   graphAddEdge(Graph *g, int src_id, int dest_id);
void  graphRemoveEdgesOf(Graph *g, int page_id);  /* hapus semua edge in/out */
/* Isi out_ids dengan page_id tujuan dari src_id, return jumlah */
int   graphGetNeighbors(Graph *g, int src_id, int *out_ids);
int   graphHasVertex(Graph *g, int page_id);
int   graphFindVertex(Graph *g, int page_id);     /* return index, -1 jika tidak ada */

#endif /* ADT_GRAPH_H */