#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "adt_simple.h"

/* Load web_pages.csv into db. Returns number of pages loaded, or -1 if file not found.
   CSV format: id,url,content  (header row optional; content may be double-quoted;
   literal \n inside content is converted to newline). */
int csvLoadWebPages(const char *path, WebDatabase *db);

/* Load linked_pages.csv into ldb. Returns number of relations loaded, or -1 if file not found.
   CSV format: id_sumber,id_tujuan  (header row optional). */
int csvLoadLinkedPages(const char *path, LinkedPagesDB *ldb);

#endif /* CSV_PARSER_H */
