#ifndef __EV_MEM__H__
#define __EV_MEM__H__

#include <stddef.h>


void *ev_zzh_malloc(size_t size);

void *ev_zzh_calloc(size_t nmemb, size_t size);

void ev_zzh_free(void *ptr);

void *ev_zzh_realloc(void *ptr, size_t size);


char *ev_zzh_strdup(const char *s);

char *ev_zzh_strndup(const char *s, size_t n);


#ifdef strdup
#undef strdup
#endif

#ifdef strndup
#undef strndup
#endif


#define calloc      ev_zzh_calloc
#define malloc      ev_zzh_malloc
#define free        ev_zzh_free
#define realloc     ev_zzh_realloc
#define strdup      ev_zzh_strdup
#define strndup     ev_zzh_strndup



#endif

