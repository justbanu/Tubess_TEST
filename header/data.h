#ifndef DATA_H
#define DATA_H

#include "adt_simple.h"
#include "adt_set.h"
#include "adt_graph.h"

/* Isi db, ldb, urlSet, dan webGraph dengan data halaman web hard-coded.
   Dipanggil sekali dari browserInit. */
void initData(WebDatabase *db, LinkedPagesDB *ldb, Set *urlSet, Graph *webGraph);

#endif /* DATA_H */
