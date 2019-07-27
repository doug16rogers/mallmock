/* Copyright (c) 2019 Doug Rogers under the Zero Clause BSD License. */
/* You are free to do whatever you want with this software. See LICENSE.txt. */

#ifndef __GNUC__
#error "This C source code must be compiled with a GNU compiler."
#endif

#include <stdlib.h>

#include "spin_lock.h"

/* @todo Just use atomic counters and bools. */
static size_t g_mallmock_any_alloc_calls = 0;
static size_t g_mallmock_any_alloc_prefail_successes = 0;
static void *g_mallmock_fail_return = NULL;
static spin_lock_t g_mallmock_lock = SPIN_LOCK_INIT_UNLOCKED;
static int g_hook_active = 0;

extern void *__libc_malloc(size_t);
extern void *__libc_calloc(size_t, size_t);
extern void *__libc_realloc(void *, size_t);

/* ------------------------------------------------------------------------- */
void *malloc(size_t size) {
    void *rval = NULL;
    int call_original = 1;
    spin_lock_acquire(&g_mallmock_lock);
    if (g_hook_active) {
        if (g_mallmock_any_alloc_calls++ == g_mallmock_any_alloc_prefail_successes) {
            call_original = 0;
        }
    }
    if (call_original) {
        rval = __libc_malloc(size);
    } else {
        rval = g_mallmock_fail_return;
    }
    spin_lock_release(&g_mallmock_lock);
    return rval;
}   /* malloc() */

/* ------------------------------------------------------------------------- */
void *calloc(size_t size, size_t nelements) {
    void *rval = NULL;
    int call_original = 1;
    spin_lock_acquire(&g_mallmock_lock);
    if (g_hook_active) {
        if (g_mallmock_any_alloc_calls++ == g_mallmock_any_alloc_prefail_successes) {
            call_original = 0;
        }
    }
    if (call_original) {
        rval = __libc_calloc(size, nelements);
    } else {
        rval = g_mallmock_fail_return;
    }
    spin_lock_release(&g_mallmock_lock);
    return rval;
}   /* calloc() */

/* ------------------------------------------------------------------------- */
void *realloc(void *ptr, size_t new_size) {
    void *rval = NULL;
    int call_original = 1;
    spin_lock_acquire(&g_mallmock_lock);
    if (g_hook_active) {
        if (g_mallmock_any_alloc_calls++ == g_mallmock_any_alloc_prefail_successes) {
            call_original = 0;
        }
    }
    if (call_original) {
        rval = __libc_realloc(ptr, new_size);
    } else {
        rval = g_mallmock_fail_return;
    }
    spin_lock_release(&g_mallmock_lock);
    return rval;
}   /* realloc() */

/* ------------------------------------------------------------------------- */
static void mallmock_unhook(void) {
    spin_lock_acquire(&g_mallmock_lock);
    g_hook_active = 0;
    spin_lock_release(&g_mallmock_lock);
}   /* mallmock_unhook() */

/* ------------------------------------------------------------------------- */
void mallmock_reset(void) {
    mallmock_unhook();
}   /* mallmock_reset() */

/* ------------------------------------------------------------------------- */
void mallmock_set_any_alloc_return(void *rval, size_t successful_returns_first) {
    spin_lock_acquire(&g_mallmock_lock);
    g_mallmock_any_alloc_calls = 0;
    g_mallmock_any_alloc_prefail_successes = successful_returns_first;
    g_mallmock_fail_return = rval;
    g_hook_active = 1;
    spin_lock_release(&g_mallmock_lock);
}   /* mallmock_set_any_alloc_return() */
