#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <event2/event.h>

#include "../evhtp-internal.h"
#include "../evhtp.h"

static struct event_base * evbase;

struct pause_ctx_ {
    struct event    * ev;
    struct timeval    tv;
    evhtp_request_t * req;
    int               count;
};

static void
test_resume(evutil_socket_t sock, short which, void * arg) {
    struct pause_ctx_ * ctx = arg;
    struct evbuffer   * buf;

    printf("%d\n", ctx->count);

    if (--ctx->count == 0) {
        evhtp_request_resume(ctx->req);
        evhtp_send_reply_chunk_end(ctx->req);

	event_free(ctx->ev);
        free(ctx);
    } else {
        buf = evbuffer_new();
        evbuffer_add_printf(buf, "%d\n", ctx->count);
        evhtp_send_reply_chunk(ctx->req, buf);

        evtimer_add(ctx->ev, &ctx->tv);
        evbuffer_free(buf);
    }
}

static void
test_pause(evhtp_request_t * r, void * args) {
    struct pause_ctx_ * ctx;

    ctx             = calloc(1, sizeof(*ctx));
    ctx->tv.tv_sec  = 1;
    ctx->tv.tv_usec = 0;
    ctx->ev         = evtimer_new(evbase, test_resume, ctx);
    ctx->req        = r;
    ctx->count      = 5;

    evtimer_add(ctx->ev, &ctx->tv);
    evhtp_send_reply_chunk_start(r, 200);

    evhtp_request_pause(r);
}

int
main(int argc, char ** argv) {
    evhtp_t * htp;

    evbase = event_base_new();
    htp    = evhtp_new(evbase, NULL);

    evhtp_set_cb(htp, "/", test_pause, NULL);
    evhtp_bind_socket(htp, "127.0.0.1", 8082, 1024);

    event_base_loop(evbase, 0);

    return 0;
}

