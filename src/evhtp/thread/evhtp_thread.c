#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#ifndef WIN32
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/queue.h>
#endif

#include <unistd.h>
#include <pthread.h>

#include <event2/event.h>

#include "evhtp/evhtp-internal.h"
#include "evhtp_thread.h"

typedef struct evhtp_thr_cmd        evhtp_thr_cmd_t;
typedef struct evhtp_thr_pool_slist evhtp_thr_pool_slist_t;

struct evhtp_thr_cmd {
    uint8_t      stop;
    void       * args;
    evhtp_thr_cb cb;
} __attribute__((packed));

TAILQ_HEAD(evhtp_thr_pool_slist, evhtp_thr_s);

struct evhtp_thr_pool_s {
#ifdef EVTHR_SHARED_PIPE
    int rdr;
    int wdr;
#endif
    int                    nthreads;
    evhtp_thr_pool_slist_t threads;
};

struct evhtp_thr_s {
    struct event_base * evbase;
    struct event      * event;
    int                 rdr;
    int                 wdr;
    char                err;
    pthread_mutex_t     lock;
    pthread_t         * thr;
    evhtp_thr_init_cb   init_cb;
    evhtp_thr_init_cb   exit_cb;
    void              * arg;
    void              * aux;

#ifdef EVTHR_SHARED_PIPE
    int            pool_rdr;
    struct event * shared_pool_ev;
#endif
    TAILQ_ENTRY(evhtp_thr) next;
};

#define t_read_(thr, cmd, sock) \
    (recv(sock, cmd, sizeof(evhtp_thr_cmd_t), 0) == sizeof(evhtp_thr_cmd_t)) ? 1 : 0

static void
t_read_cmd_(evutil_socket_t sock, short which, void * args) {
    evhtp_thr     * thread;
    evhtp_thr_cmd_t cmd;
    int             stopped;

    if (!(thread = (evhtp_thr *)args)) {
        return;
    }

    stopped = 0;

    if (evhtp_likely(t_read_(thread, &cmd, sock) == 1)) {
        stopped = cmd.stop;

        if (evhtp_likely(cmd.cb != NULL)) {
            (cmd.cb)(thread, cmd.args, thread->arg);
        }
    }

    if (evhtp_unlikely(stopped == 1)) {
        event_base_loopbreak(thread->evbase);
    }

    return;
}

static void *
t_loop_(void * args) {
    evhtp_thr * thread;

    if (!(thread = (evhtp_thr *)args)) {
        return NULL;
    }

    if (thread == NULL || thread->thr == NULL) {
        pthread_exit(NULL);
    }

    thread->evbase = event_base_new();
    thread->event  = event_new(thread->evbase,
                               thread->rdr,
                               EV_READ | EV_PERSIST,
                               t_read_cmd_, args);

    event_add(thread->event, NULL);

#ifdef EVTHR_SHARED_PIPE
    if (thread->pool_rdr > 0) {
        thread->shared_pool_ev = event_new(thread->evbase,
                                           thread->pool_rdr,
                                           EV_READ | EV_PERSIST,
                                           t_read_cmd_, args);

        event_add(thread->shared_pool_ev, NULL);
    }
#endif

    pthread_mutex_lock(&thread->lock);
    {
        if (thread->init_cb != NULL) {
            (thread->init_cb)(thread, thread->arg);
        }
    }
    pthread_mutex_unlock(&thread->lock);

    event_base_loop(thread->evbase, 0);

    pthread_mutex_lock(&thread->lock);
    {
        if (thread->exit_cb != NULL) {
            (thread->exit_cb)(thread, thread->arg);
        }
    }
    pthread_mutex_unlock(&thread->lock);

    if (thread->err == 1) {
        fprintf(stderr, "FATAL ERROR!\n");
    }

    pthread_exit(NULL);
} /* t_loop_ */

static evhtp_thr *
t_new_(evhtp_thr_init_cb init_cb, evhtp_thr_exit_cb exit_cb, void * args) {
    evhtp_thr * thread;
    int         fds[2];

    if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1) {
        return NULL;
    }

    evutil_make_socket_nonblocking(fds[0]);
    evutil_make_socket_nonblocking(fds[1]);

    if (!(thread = calloc(sizeof(evhtp_thr), 1))) {
        return NULL;
    }

    thread->thr = malloc(sizeof(pthread_t));
    thread->arg = args;
    thread->rdr = fds[0];
    thread->wdr = fds[1];

    evhtp_thr_set_initcb(thread, init_cb);
    evhtp_thr_set_exitcb(thread, exit_cb);

    if (pthread_mutex_init(&thread->lock, NULL)) {
        evhtp_thr_free(thread);
        return NULL;
    }

    return thread;
}

evhtp_thr_res
evhtp_thr_defer(evhtp_thr * thread, evhtp_thr_cb cb, void * arg) {
    evhtp_thr_cmd_t cmd = {
        .cb   = cb,
        .args = arg,
        .stop = 0
    };

    if (send(thread->wdr, &cmd, sizeof(cmd), 0) <= 0) {
        return EVTHR_RES_RETRY;
    }

    return EVTHR_RES_OK;
}

evhtp_thr_res
evhtp_thr_stop(evhtp_thr * thread) {
    evhtp_thr_cmd_t cmd = {
        .cb   = NULL,
        .args = NULL,
        .stop = 1
    };

    if (send(thread->wdr, &cmd, sizeof(evhtp_thr_cmd_t), 0) < 0) {
        return EVTHR_RES_RETRY;
    }

    pthread_join(*thread->thr, NULL);
    return EVTHR_RES_OK;
}

struct event_base *
evhtp_thr_get_base(evhtp_thr * thr) {
    return thr ? thr->evbase : NULL;
}

void
evhtp_thr_set_aux(evhtp_thr * thr, void * aux) {
    if (thr) {
        thr->aux = aux;
    }
}

void *
evhtp_thr_get_aux(evhtp_thr * thr) {
    return thr ? thr->aux : NULL;
}

int
evhtp_thr_set_initcb(evhtp_thr * thr, evhtp_thr_init_cb cb) {
    if (thr == NULL) {
        return -1;
    }

    thr->init_cb = cb;

    return 01;
}

int
evhtp_thr_set_exitcb(evhtp_thr * thr, evhtp_thr_exit_cb cb) {
    if (thr == NULL) {
        return -1;
    }

    thr->exit_cb = cb;

    return 0;
}

evhtp_thr *
evhtr_new(evhtp_thr_init_cb init_cb, void * args) {
    return t_new_(init_cb, NULL, args);
}

evhtp_thr *
evhtp_thr_wexit_new(evhtp_thr_init_cb init_cb, evhtp_thr_exit_cb exit_cb, void * args) {
    return t_new_(init_cb, exit_cb, args);
}

int
evhtp_thr_start(evhtp_thr * thread) {
    if (thread == NULL || thread->thr == NULL) {
        return -1;
    }

    if (pthread_create(thread->thr, NULL, t_loop_, (void *)thread)) {
        return -1;
    }

    return 0;
}

void
evhtp_thr_free(evhtp_thr * thread) {
    if (thread == NULL) {
        return;
    }

    if (thread->rdr > 0) {
        close(thread->rdr);
    }

    if (thread->wdr > 0) {
        close(thread->wdr);
    }

    if (thread->thr) {
        free(thread->thr);
    }

    if (thread->event) {
        event_free(thread->event);
    }

    if (thread->evbase) {
        event_base_free(thread->evbase);
    }

    free(thread);
} /* evhtp_thr_free */

void
evhtp_thr_pool_free(evhtp_thr_pool_t * pool) {
    evhtp_thr * thread;
    evhtp_thr * save;

    if (pool == NULL) {
        return;
    }

    TAILQ_FOREACH_SAFE(thread, &pool->threads, next, save) {
        TAILQ_REMOVE(&pool->threads, thread, next);

        evhtp_thr_free(thread);
    }

    free(pool);
}

evhtp_thr_res
evhtp_thr_pool_stop(evhtp_thr_pool_t * pool) {
    evhtp_thr * thr;
    evhtp_thr * save;

    if (pool == NULL) {
        return EVTHR_RES_FATAL;
    }

    TAILQ_FOREACH_SAFE(thr, &pool->threads, next, save) {
        evhtp_thr_stop(thr);
    }

    return EVTHR_RES_OK;
}

evhtp_thr_res
evhtp_thr_pool_defer(evhtp_thr_pool_t * pool, evhtp_thr_cb cb, void * arg) {
#ifdef EVTHR_SHARED_PIPE
    evhtp_thr_cmd_t cmd = {
        .cb   = cb,
        .args = arg,
        .stop = 0
    };

    if (evhtp_unlikely(send(pool->wdr, &cmd, sizeof(cmd), 0) == -1)) {
        return EVTHR_RES_RETRY;
    }

    return EVTHR_RES_OK;
#else
    evhtp_thr * thr = NULL;

    if (pool == NULL) {
        return EVTHR_RES_FATAL;
    }

    if (cb == NULL) {
        return EVTHR_RES_NOCB;
    }

    thr = TAILQ_FIRST(&pool->threads);

    TAILQ_REMOVE(&pool->threads, thr, next);
    TAILQ_INSERT_TAIL(&pool->threads, thr, next);


    return evhtp_thr_defer(thr, cb, arg);
#endif
} /* evhtp_thr_pool_defer */

static evhtp_thr_pool_t *
t_pool_new_(int               nthreads,
            evhtp_thr_init_cb init_cb,
            evhtp_thr_exit_cb exit_cb,
            void            * shared) {
    evhtp_thr_pool_t * pool;
    int                i;

#ifdef EVTHR_SHARED_PIPE
    int fds[2];
#endif

    if (nthreads == 0) {
        return NULL;
    }

    if (!(pool = calloc(sizeof(evhtp_thr_pool_t), 1))) {
        return NULL;
    }

    pool->nthreads = nthreads;
    TAILQ_INIT(&pool->threads);

#ifdef EVTHR_SHARED_PIPE
    if (evutil_socketpair(AF_UNIX, SOCK_DGRAM, 0, fds) == -1) {
        return NULL;
    }

    evutil_make_socket_nonblocking(fds[0]);
    evutil_make_socket_nonblocking(fds[1]);

    pool->rdr = fds[0];
    pool->wdr = fds[1];
#endif

    for (i = 0; i < nthreads; i++) {
        evhtp_thr * thread;

        if (!(thread = evhtp_thr_wexit_new(init_cb, exit_cb, shared))) {
            evhtp_thr_pool_free(pool);
            return NULL;
        }

#ifdef EVTHR_SHARED_PIPE
        thread->pool_rdr = fds[0];
#endif

        TAILQ_INSERT_TAIL(&pool->threads, thread, next);
    }

    return pool;
} /* t_pool_new_ */

evhtp_thr_pool_t *
evhtp_thr_pool_new(int nthreads, evhtp_thr_init_cb init_cb, void * shared) {
    return t_pool_new_(nthreads, init_cb, NULL, shared);
}

evhtp_thr_pool_t *
evhtp_thr_pool_wexit_new(int nthreads,
                         evhtp_thr_init_cb init_cb,
                         evhtp_thr_exit_cb exit_cb, void * shared) {
    return t_pool_new_(nthreads, init_cb, exit_cb, shared);
}

int
evhtp_thr_pool_start(evhtp_thr_pool_t * pool) {
    evhtp_thr * evhtp_thr = NULL;

    if (pool == NULL) {
        return -1;
    }

    TAILQ_FOREACH(evhtp_thr, &pool->threads, next) {
        if (evhtp_thr_start(evhtp_thr) < 0) {
            return -1;
        }

        usleep(5000);
    }

    return 0;
}

