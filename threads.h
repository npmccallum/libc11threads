/*
 * libc11threads - Wrapper library for C11's <threads.h>
 *
 * Copyright 2011 Nathaniel McCallum <nathaniel@themccallums.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <time.h>

#include <pthread.h>
#include <stdnoreturn.h>

#define thread_local __thread
#define ONCE_FLAG_INIT PTHREAD_ONCE_INIT
#define TSS_DTOR_ITERATIONS PTHREAD_DESTRUCTOR_ITERATIONS

typedef pthread_cond_t cnd_t;
typedef pthread_t thrd_t;
typedef pthread_key_t tss_t;
typedef pthread_mutex_t mtx_t;
typedef pthread_once_t once_flag;

typedef void
(*tss_dtor_t)(void*);

typedef int
(*thrd_start_t)(void*);

enum {
  mtx_plain = 0,
  mtx_timed = (1 << 0),
  mtx_recursive = (1 << 1)
};

enum {
  thrd_success = 0, thrd_timedout, thrd_busy, thrd_error, thrd_nomem
};

void
call_once(once_flag *flag, void (*func)(void))
  __attribute__((nonnull));

int
cnd_broadcast(cnd_t *cond)
  __attribute__((nonnull));

void
cnd_destroy(cnd_t *cond)
  __attribute__((nonnull));

int
cnd_init(cnd_t *cond)
  __attribute__((nonnull));

int
cnd_signal(cnd_t *cond)
  __attribute__((nonnull));

int
cnd_timedwait(cnd_t *cond, mtx_t *mtx, const struct timespec *ts)
  __attribute__((nonnull));

int
cnd_wait(cnd_t *cond, mtx_t *mtx)
  __attribute__((nonnull));

void
mtx_destroy(mtx_t *mtx)
  __attribute__((nonnull));

int
mtx_init(mtx_t *mtx, int type)
  __attribute__((nonnull));

int
mtx_lock(mtx_t *mtx)
  __attribute__((nonnull));

int
mtx_timedlock(mtx_t *mtx, const struct timespec *ts)
  __attribute__((nonnull));

int
mtx_trylock(mtx_t *mtx)
  __attribute__((nonnull));

int
mtx_unlock(mtx_t *mtx)
  __attribute__((nonnull));

int
thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
  __attribute__((nonnull (1, 2)));

thrd_t
thrd_current(void);

int
thrd_detach(thrd_t thr);

int
thrd_equal(thrd_t thr0, thrd_t thr1);

noreturn void
thrd_exit(int res);

int
thrd_join(thrd_t thr, int *res);

int
thrd_sleep(const struct timespec *duration, struct timespec *remaining);

void
thrd_yield(void);

int
tss_create(tss_t *key, tss_dtor_t dtor)
  __attribute__((nonnull (1)));

void
tss_delete(tss_t key);

void *
tss_get(tss_t key);

int
tss_set(tss_t key, void *val);
