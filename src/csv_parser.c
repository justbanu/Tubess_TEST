#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../header/csv_parser.h"

/* ----------------------------------------------------------------
   Internal helpers
   ---------------------------------------------------------------- */

/* Skip a header row if the first field is non-numeric (e.g. "id"). */
static void skipHeaderIfPresent(FILE *f) {
    int c = fgetc(f);
    if (c == EOF) return;
    if (isdigit(c)) {
        /* First char is a digit — this is a data row; put it back. */
        ungetc(c, f);
        return;
    }
    /* Skip until end of this line. */
    while (c != '\n' && c != EOF) c = fgetc(f);
}

/* Read one CSV field (comma-delimited, optional double-quote wrapping).
   Stops at ',' (returns 1) or '\n'/EOF (returns 0).
   Literal two-char sequence '\''n' inside the field is converted to newline. */
static int readField(FILE *f, char *out, int maxlen) {
    int c = fgetc(f);
    if (c == EOF) { out[0] = '\0'; return -1; }

    int len = 0;
    int quoted = (c == '"');

    if (quoted) {
        /* Quoted field: read until closing '"', '""' is an escaped quote. */
        while (1) {
            c = fgetc(f);
            if (c == EOF) break;
            if (c == '"') {
                int next = fgetc(f);
                if (next == '"') {
                    /* Escaped double-quote */
                    if (len < maxlen - 1) out[len++] = '"';
                } else {
                    /* End of quoted field */
                    if (next == ',')                 { out[len] = '\0'; return 1; }
                    if (next == '\n' || next == EOF) { out[len] = '\0'; return 0; }
                    if (next == '\r') {
                        int after = fgetc(f);
                        if (after != '\n') ungetc(after, f);
                        out[len] = '\0'; return 0;
                    }
                    /* Anything else: ignore and keep going. */
                }
            } else {
                if (len < maxlen - 1) out[len++] = c;
            }
        }
    } else {
        /* Unquoted field: read until comma, newline, or EOF. */
        while (c != ',' && c != '\n' && c != '\r' && c != EOF) {
            if (len < maxlen - 1) out[len++] = c;
            c = fgetc(f);
        }
        if (c == '\r') {
            int next = fgetc(f);
            if (next != '\n') ungetc(next, f);
            c = '\n';
        }
        out[len] = '\0';
        return (c == ',') ? 1 : 0;
    }

    out[len] = '\0';
    return 0;
}

/* Convert literal two-char '\''n' sequences in buf to actual newline characters. */
static void unescapeNewlines(char *buf) {
    char tmp[MAX_CONTENT_LEN];
    int r = 0, w = 0;
    while (buf[r]) {
        if (buf[r] == '\\' && buf[r + 1] == 'n') {
            tmp[w++] = '\n';
            r += 2;
        } else {
            tmp[w++] = buf[r++];
        }
    }
    tmp[w] = '\0';
    strncpy(buf, tmp, MAX_CONTENT_LEN - 1);
    buf[MAX_CONTENT_LEN - 1] = '\0';
}

/* ----------------------------------------------------------------
   Public API
   ---------------------------------------------------------------- */

int csvLoadWebPages(const char *path, WebDatabase *db) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    skipHeaderIfPresent(f);

    int count = 0;
    char field_id[32];
    char field_url[MAX_URL_LEN];
    char field_content[MAX_CONTENT_LEN];

    while (1) {
        int r;

        /* Field 1: id */
        r = readField(f, field_id, sizeof(field_id));
        if (r == -1) break;   /* EOF before any field */
        if (field_id[0] == '\0') break; /* blank line */

        /* Field 2: url */
        if (r == 0) { field_url[0] = '\0'; field_content[0] = '\0'; goto add; }
        r = readField(f, field_url, sizeof(field_url));

        /* Field 3: content */
        if (r == 0) { field_content[0] = '\0'; goto add; }
        r = readField(f, field_content, sizeof(field_content));
        /* Consume any remaining fields on this row */
        while (r == 1) {
            char discard[64];
            r = readField(f, discard, sizeof(discard));
        }

        unescapeNewlines(field_content);

add:
        if (field_url[0] == '\0') continue;

        /* Force the auto-increment to assign the ID from the CSV. */
        int csv_id = atoi(field_id);
        if (csv_id > 0) db->next_id = csv_id;

        int assigned = addWebPage(db, field_url, field_content);
        if (assigned > 0) count++;
    }

    fclose(f);
    return count;
}

int csvLoadLinkedPages(const char *path, LinkedPagesDB *ldb) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    skipHeaderIfPresent(f);

    int count = 0;
    char f1[32], f2[32];

    while (1) {
        int r = readField(f, f1, sizeof(f1));
        if (r == -1) break;
        if (f1[0] == '\0') break;

        if (r == 0) continue; /* only one field on this line — skip */
        r = readField(f, f2, sizeof(f2));

        /* Consume any remaining fields */
        while (r == 1) {
            char discard[64];
            r = readField(f, discard, sizeof(discard));
        }

        int src = atoi(f1);
        int dst = atoi(f2);
        if (src > 0 && dst > 0) {
            addLinkedPage(ldb, src, dst);
            count++;
        }
    }

    fclose(f);
    return count;
}
