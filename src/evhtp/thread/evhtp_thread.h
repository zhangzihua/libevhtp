#ifndef __EVHTP_THR_H__
#define __EVHTP_THR_H__

#include <pthread.h>
#include <event2/event.h>
#include <evhtp-config.h>

#ifdef __cplusplus
extern "C" {
#endif

enum evhtp_thr_res {
    EVTHR_RES_OK = 0,
    EVTHR_RES_BACKLOG,
    EVTHR_RES_RETRY,
    EVTHR_RES_NOCB,
    EVTHR_RES_FATAL
};

struct evhtp_thr_pool_s;
struct evhtp_thr_s;

typedef struct evhtp_thr_pool evhtp_thr_pool;
typedef struct evhtp_thr      evhtp_thr;
typedef enum evhtp_thr_res    evhtp_thr_res;

typedef void (* evhtp_thr_cb)(evhtp_thr * thr, void * cmd_arg, void * shared);
typedef void (* evhtp_thr_init_cb)(evhtp_thr * thr, void * shared);
typedef void (* evhtp_thr_exit_cb)(evhtp_thr * thr, void * shared);


/**
 * @brief creates a single new thread
 * @note to be deprecated by evhtp_thr_wexit_new
 *
 * @param evhtp_thr_init_cb callback to execute upon spawn
 * @param shared data which is passed to every callback
 *
 * @return NULL on error
 * @see evhtp_thr_free(), evhtp_thr_wexit_new()
 */
EVHTP_EXPORT
evhtp_thr * evhtp_thr_new(evhtp_thr_init_cb, void *)
DEPRECATED("will take on the syntax of evhtp_thr_wexit_new");


/**
 * @brief same as evhtp_thr_new() but execs a callback on a thread exit
 *
 * @param evhtp_thr_init_cb
 * @param evhtp_thr_exit_cb
 * @param shared
 *
 * @return
 * @see evhtp_thr_new()
 */
EVHTP_EXPORT
evhtp_thr * evhtp_thr_wexit_new(
    evhtp_thr_init_cb,
    evhtp_thr_exit_cb,
    void * shared);


/**
 * @brief free all underlying data in a ssingle evhtp_thr
 *
 * @param evhtp_thr
 *
 * @return
 * @see evhtp_thr_new(), evhtp_thr_wexit_new()
 */
EVHTP_EXPORT
void evhtp_thr_free(evhtp_thr * evhtp_thr);

/**
 * @brief get the thread-specific event_base
 *
 * @param thr a single thread
 *
 * @return the event_base of the thread, NULL on error
 */
EVHTP_EXPORT
struct event_base * evhtp_thr_get_base(evhtp_thr * thr);

/**
 * @brief set non-shared thread-specific arguments
 *
 * @param thr the thread context
 * @param aux the data to set
 *
 * @return
 * @see evhtp_thr_get_aux()
 */
EVHTP_EXPORT
void evhtp_thr_set_aux(evhtp_thr * thr, void * aux);


/**
 * @brief get the non-shared thread arguments
 *
 * @param thr a single thread context
 *
 * @return the threads non-shared arguments
 * @see evhtp_thr_set_aux()
 */
EVHTP_EXPORT
void * evhtp_thr_get_aux(evhtp_thr * thr);


/**
 * @brief starts up the thread + event loop
 *
 * @param evhtp_thr
 *
 * @return 0 on success, -1 on error
 * @see evhtp_thr_stop()
 */
EVHTP_EXPORT
int evhtp_thr_start(evhtp_thr * evhtp_thr);


/**
 * @brief stop and shutdown a thread
 *
 * @param evhtp_thr
 *
 * @return an evhtp_thr_res
 * @see evhtp_thr_start()
 */
EVHTP_EXPORT
evhtp_thr_res evhtp_thr_stop(evhtp_thr * evhtp_thr);


/**
 * @brief defer a callback into a thread
 * @note any shared data needs to be reentrant
 *
 * @param evhtp_thr the evhtp_thread context
 * @param cb callback to execute in the thread
 * @param args arguments to be passed to the callback
 *
 * @return evhtp_thr_res
 */
EVHTP_EXPORT
evhtp_thr_res evhtp_thr_defer(
    evhtp_thr * evhtp_thr,
    evhtp_thr_cb cb, void *);

/**
 * @brief create a new threadpool
 * @note deprecated bty evhtp_thr_pool_wexit_new
 *
 * @param nthreads number of threads
 * @param evhtp_thr_init_cb callback to execute on a new spawn
 * @param shared args passed to the callback
 *
 * @return a evhtp_thr_pool on success, NULL on error
 * @see evhtp_thr_pool_free(), evhtp_thr_pool_wexit_new()
 */
EVHTP_EXPORT
evhtp_thr_pool * evhtp_thr_pool_new(int nthreads, evhtp_thr_init_cb, void *)
DEPRECATED("will take on the syntax of evhtp_thr_pool_wexit_new");


/**
 * @brief works like evhtp_thr_pool_new, but holds a thread on-exit callback
 *
 * @param nthreads number of threads
 * @param evhtp_thr_init_cb thread on-init callback
 * @param evhtp_thr_exit_cb thread on-exit callback
 * @param shared args passed to the callback
 *
 * @return evhtp_thr_pool on success, NULL on error
 * @see evhtp_thr_pool_free(), evhtp_thr_pool_new()
 */
EVHTP_EXPORT
evhtp_thr_pool * evhtp_thr_pool_wexit_new(
    int nthreads,
    evhtp_thr_init_cb,
    evhtp_thr_exit_cb, void *);


/**
 * @brief iterates over each thread and tears it down
 * @note if the on-exit cb is set, it is called for each thread
 *
 * @param pool
 *
 * @return
 * @see evhtp_thr_pool_new(), evhtp_thr_pool_wexit_new()
 */
EVHTP_EXPORT
void evhtp_thr_pool_free(evhtp_thr_pool * pool);


/**
 * @brief spawns all the threads in the pool
 *
 * @param pool
 *
 * @return 0 on success, -1 on error
 * @see evhtp_thr_pool_stop()
 */
EVHTP_EXPORT
int evhtp_thr_pool_start(evhtp_thr_pool * pool);


/**
 * @brief stops all threads in the pool
 *
 * @param pool
 *
 * @return evhtp_thr res
 * @see evhtp_thr_pool_start()
 */
EVHTP_EXPORT
evhtp_thr_res evhtp_thr_pool_stop(evhtp_thr_pool * pool);


/**
 * @brief execute a callback using a thread in the pool
 *
 * @param pool the threadpoool context
 * @param cb callback to execute in a single thread
 * @param arg argument passed to the callback
 *
 * @return res
 * @see evhtp_thr_defer()
 */
EVHTP_EXPORT
evhtp_thr_res evhtp_thr_pool_defer(
    evhtp_thr_pool * pool,
    evhtp_thr_cb     cb,
    void           * arg);


#ifdef __cplusplus
}
#endif

#endif /* __EVTHR_H__ */

