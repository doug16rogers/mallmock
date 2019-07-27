/* Copyright (c) 2019 Doug Rogers under the Zero Clause BSD License. */
/* You are free to do whatever you want with this software. See LICENSE.txt. */

#ifndef LIB_SPIN_LOCK_H_
#define LIB_SPIN_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef int32_t spin_lock_t;

#define SPIN_LOCK_INIT_UNLOCKED (0)
#define SPIN_LOCK_INIT_LOCKED   (1)

static inline void spin_lock_acquire(spin_lock_t* sp) {
    while (!__sync_bool_compare_and_swap(sp, 0, 1)) {
    }
}

static inline void spin_lock_release(spin_lock_t* sp) {
    *sp = 0;
}

#ifdef __cplusplus
}
#endif

#endif  // LIB_SPIN_LOCK_H_
