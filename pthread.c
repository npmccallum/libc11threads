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

#include "threads.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define twoway(s) return (s) == 0 ? thrd_success : thrd_error
#define threeway(s, e, r) \
  switch (s) { \
  case 0: \
    return thrd_success; \
  case e: \
    return r; \
  default: \
    return thrd_error; \
  }

/* 7.25.2.1 */
void
call_once(once_flag *flag, void (*func)(void))
{
  pthread_once(flag, func);
}

/* 7.25.3.1 */
int
cnd_broadcast(cnd_t *cond)
{
  twoway(pthread_cond_broadcast(cond));
}

/* 7.25.3.2 */
void
cnd_destroy(cnd_t *cond)
{
  pthread_cond_destroy(cond);
}

/* 7.25.3.3 */
int
cnd_init(cnd_t *cond)
{
  threeway(pthread_cond_init(cond, NULL), ENOMEM, thrd_nomem);
}

/* 7.25.3.4 */
int
cnd_signal(cnd_t *cond)
{
  twoway(pthread_cond_signal(cond));
}

/* 7.25.3.5 */
int
cnd_timedwait(cnd_t *cond, mtx_t *mtx, const struct timespec *ts)
{
  threeway(pthread_cond_timedwait(cond, mtx, ts), ETIMEDOUT, thrd_busy);
}

/* 7.25.3.6 */
int
cnd_wait(cnd_t *cond, mtx_t *mtx)
{
  twoway(pthread_cond_wait(cond, mtx));
}

/* 7.25.4.1 */
void
mtx_destroy(mtx_t *mtx)
{
  pthread_mutex_destroy(mtx);
}

/* 7.25.4.2 */
int
mtx_init(mtx_t *mtx, int type)
{
  pthread_mutexattr_t attr;
  int status;

  switch (type) {
  case mtx_plain:
  case mtx_timed:
  case mtx_plain | mtx_recursive:
  case mtx_timed | mtx_recursive:
    break;
  default:
    return thrd_error;
  }

  if (pthread_mutexattr_init(&attr) != 0)
    return thrd_error;

  if (type & mtx_recursive)
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

  status = pthread_mutex_init(mtx, &attr);
  pthread_mutexattr_destroy(&attr);
  twoway(status);
}

/* 7.25.4.3 */
int
mtx_lock(mtx_t *mtx)
{
  twoway(pthread_mutex_lock(mtx));
}

/* 7.25.4.4 */
int
mtx_timedlock(mtx_t *mtx, const struct timespec *ts)
{
  threeway(pthread_mutex_timedlock(mtx, ts), ETIMEDOUT, thrd_busy);
}

/* 7.25.4.5 */
int
mtx_trylock(mtx_t *mtx)
{
  twoway(pthread_mutex_trylock(mtx));
}

/* 7.25.4.6 */
int
mtx_unlock(mtx_t *mtx)
{
  twoway(pthread_mutex_unlock(mtx));
}

/* 7.25.5.1 */
struct wrapper_data {
  thrd_start_t func;
  void *arg;
};

static void *
wrapper(void *d)
{
  struct wrapper_data data = *(struct wrapper_data *)d;

  free(d);
  return (void*) (intptr_t) data.func(data.arg);
}

int
thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
  struct wrapper_data *data;

  data = malloc(sizeof(struct wrapper_data));
  if (!data)
    return thrd_nomem;

  data->func = func;
  data->arg = arg;

  if (pthread_create(thr, NULL, wrapper, data) != 0) {
    free(data);
    return thrd_error;
  }

  return thrd_success;
}

/* 7.25.5.2 */
thrd_t
thrd_current(void)
{
  return pthread_self();
}

/* 7.25.5.3 */
int
thrd_detach(thrd_t thr)
{
  twoway(pthread_detach(thr));
}

/* 7.25.5.4 */
int
thrd_equal(thrd_t thr0, thrd_t thr1)
{
  return pthread_equal(thr0, thr1);
}

/* 7.25.5.5 */
noreturn void
thrd_exit(int res)
{
  pthread_exit((void*) (intptr_t) res);
}

/* 7.25.5.6 */
int
thrd_join(thrd_t thr, int *res)
{
  void *tmp;

  if (pthread_join(thr, &tmp) == 0) {
    if (res)
      *res = (int) (intptr_t) tmp;
    return thrd_success;
  }

  return thrd_error;
}

/* 7.25.5.7 */
int
thrd_sleep(const struct timespec *duration, struct timespec *remaining)
{
  if (nanosleep(duration, remaining) < 0)
    return errno == EINTR ? -1 : -2;
  return 0;
}

/* 7.25.5.8 */
void
thrd_yield(void)
{
  sched_yield();
}

/* 7.25.6.1 */
int
tss_create(tss_t *key, tss_dtor_t dtor)
{
  twoway(pthread_key_create(key, dtor));
}

/* 7.25.6.2 */
void
tss_delete(tss_t key)
{
  pthread_key_delete(key);
}

/* 7.25.6.3 */
void *
tss_get(tss_t key)
{
  return pthread_getspecific(key);
}

/* 7.25.6.4 */
int
tss_set(tss_t key, void *val)
{
  twoway(pthread_setspecific(key, val));
}
