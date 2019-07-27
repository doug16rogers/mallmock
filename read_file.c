/* Copyright (c) 2019 Doug Rogers under the Zero Clause BSD License. */
/* You are free to do whatever you want with this software. See LICENSE.txt. */

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "link_list.h"
#include "read_file.h"

/**
 * A single line consists of a link for use in a linked list and a pointer to
 * a heap-allocated line.
 */
typedef struct line_item_s {
    link_t link;  /**< Link for placing in main list. */
    char line[0]; /**< NUL-terminated characters for this line. */
} line_item_t;

/**
 * A read-file object consists of a 
 */
struct read_file_s {
    char *filename;    /**< Name of file that was opened; on the heap. */
    size_t line_count; /**< Number of lines read from file. */
    list_t line_list;  /**< Linked list of lines, each on the heap. */
};

/* ------------------------------------------------------------------------- */
/**
 * Add a line to the read_file_t @p f.
 *
 * @return 1 on success, 0 on failure.
 */
static int read_file_add_line(read_file_t *f, const char *line, size_t size) {
    line_item_t *li = NULL;

    assert(NULL != f);
    assert(NULL != line);
    assert(size > 0);

    /* Allocate line item struct plus line plus room for terminator. */
    li = calloc(1, sizeof(struct line_item_s) + size + 1);
    if (NULL == li) {
        return 0;
    }
    memcpy(li->line, line, size);
    list_insert_prev(&f->line_list, &li->link);
    f->line_count++;
    return 1;
}   /* read_file_add_line() */

/* ------------------------------------------------------------------------- */
read_file_t *read_file_new(const char *filename) {
    read_file_t *f = NULL;
    int fd = -1;
    char buf[0x400];
    size_t read_index = 0;
    ssize_t bytes_read = 0;

    if (NULL == filename) {
        return NULL;
    }
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }
    f = calloc(1, sizeof(struct read_file_s));
    if (NULL == f) {
        goto Error;
    }
    list_init(&f->line_list);
    f->filename = strdup(filename);
    if (NULL == f->filename) {
        goto Error;
    }

    while ((bytes_read = read(fd, &buf[read_index], sizeof(buf) - read_index)) > 0) {
        size_t start = 0;
        size_t i;

        for (i = read_index; i < read_index + bytes_read; ++i) {
            if ('\n' == buf[i]) {
                if (!read_file_add_line(f, &buf[start], i + 1 - start)) {
                    goto Error;
                }
                start = i + 1;
            }
        }
        if ((sizeof(buf) == i) && (0 == start)) {
            /*
             * No newline found in full buffer. Treat it all as a line. It's
             * just test code! :)
             */
            if (!read_file_add_line(f, buf, bytes_read)) {
                goto Error;
            }
            read_index = 0;
        } else {
            /* Some leftover. Slide those to the start of the buffer. */
            size_t bytes_left = read_index + bytes_read - start;
            memmove(&buf[0], &buf[start], bytes_left); /* Might overlap, so use memmove(). */
            read_index = bytes_left;
        }
    }

    if (read_index > 0) {
        if (!read_file_add_line(f, buf, read_index)) {
            goto Error;
        }
    }

    close(fd);
    return f;

Error:
    if (NULL != f) {
        read_file_delete(f);
    }
    if (fd >= 0) {
        close(fd);
    }
    return NULL;
}   /* read_file_new() */

/* ------------------------------------------------------------------------- */
void read_file_delete(read_file_t *f) {
    if (NULL != f) {
        list_foreach_struct(&f->line_list, line_item, line_item_t, link,
                            link_remove(&line_item->link);
                            free(line_item));
        if (NULL != f->filename) {
            free(f->filename);
            f->filename = NULL;
        }
        free(f);
    }
}   /* read_file_delete() */

/* ------------------------------------------------------------------------- */
void read_file_delete_null(read_file_t **f_ptr) {
    if (NULL != f_ptr) {
        read_file_delete(*f_ptr);
        *f_ptr = NULL;
    }
}   /* read_file_delete_null() */

/* ------------------------------------------------------------------------- */
const char *read_file_get_filename(read_file_t *f) {
    if (NULL == f) {
        return NULL;
    }
    return f->filename;
}   /* read_file_get_filename() */

/* ------------------------------------------------------------------------- */
size_t read_file_get_line_count(read_file_t *f) {
    if (NULL == f) {
        return 0;
    }
    return f->line_count;
}   /* read_file_get_line_count() */

/* ------------------------------------------------------------------------- */
const char *read_file_get_line(read_file_t *f, size_t n) {
    if (NULL == f) {
        return NULL;
    }
    list_foreach_struct(&f->line_list, line_item, line_item_t, link,
                        if (0 == n) {
                            return line_item->line;
                        }
                        n--;
        );
    return NULL;
}   /* read_file_get_line() */
