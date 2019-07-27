/* Copyright (c) 2019 Doug Rogers under the Zero Clause BSD License. */
/* You are free to do whatever you want with this software. See LICENSE.txt. */

#ifndef MALLMOCK_MALLMOCK_H_
#define MALLMOCK_MALLMOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * Reset - always call through to libc's allocation functions.
 */
void mallmock_reset(void);

#if 0
/**
 * Have malloc() return @p rval after first succeeding @p
 * successful_returns_first times.
 */
void mallmock_set_malloc_return(void *rval, size_t successful_returns_first);
#endif

/**
 * Have malloc()/calloc()/realloc() return @p rval after first succeeding @p
 * successful_returns_first times.
 */
void mallmock_set_any_alloc_return(void *rval, size_t successful_returns_first);

#ifdef __cplusplus
}
#endif

#endif  // MALLMOCK_MALLMOCK_H_
