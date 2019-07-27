/* Copyright (c) 2019 Doug Rogers under the Zero Clause BSD License. */
/* You are free to do whatever you want with this software. See LICENSE.txt. */

#ifndef MALLMOCK_READ_FILE_H_
#define MALLMOCK_READ_FILE_H_

/*
 * This is a sample unit under test. It provides a means to read the contents
 * of a file as a set of lines.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct read_file_s read_file_t;

/**
 * Read the contents of @p filename as a set of lines.
 *
 * @return a pointer to a read-file object containing all the lines of @p
 * filename, or `NULL` on error.
 */
read_file_t *read_file_new(const char *filename);

/**
 * Dispose of the read-file object @p f returned by `read_file_new()`.
 */
void read_file_delete(read_file_t *f);

/**
 * Call `read_file_delete(*f_ptr)` then set `*f_ptr = NULL`.
 */
void read_file_delete_null(read_file_t **f_ptr);

/**
 * @return the filename used (but copied) in `read_file_new()`, or `NULL` on
 * error.
 */
const char *read_file_get_filename(read_file_t *f);

/**
 * @return the number of lines loaded from @p f, or 0 on error.
 */
size_t read_file_get_line_count(read_file_t *f);

/**
 * @return a pointer to the (0-based) @p n-th line from @p f, or `NULL` on
 * error.
 */
const char *read_file_get_line(read_file_t *f, size_t n);

#ifdef __cplusplus
}
#endif

#endif  // MALLMOCK_READ_FILE_H_
