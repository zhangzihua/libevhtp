

#include <stdlib.h>

#include "evhtp.h"


static void *(*_zzh_malloc_fn)(size_t sz) = NULL;

static void *(*_zzh_realloc_fn)(void *p, size_t sz) = NULL;


static void (*_zzh_free_fn)(void *p) = NULL;



void zzh_set_mem_functions(
	void *(*malloc_fn)(size_t sz),
	void *(*realloc_fn)(void *ptr, size_t sz),
	void (*free_fn)(void *ptr))
{
    _zzh_malloc_fn = malloc_fn;
    _zzh_realloc_fn = realloc_fn;
    _zzh_free_fn = free_fn;
}


void *ev_zzh_malloc(size_t size)
{
    if(_zzh_malloc_fn != NULL)
        return _zzh_malloc_fn(size);
    else    
        return malloc(size);
}

void ev_zzh_free(void *ptr)
{
	if (_zzh_free_fn)
		_zzh_free_fn(ptr);
	else
		free(ptr);
    
}

void *ev_zzh_realloc(void *ptr, size_t size)
{
    if(_zzh_realloc_fn != NULL)
        return _zzh_realloc_fn(ptr, size);
    else    
        return realloc(ptr, size);
    
}



void *ev_zzh_calloc(size_t nmemb, size_t size)
{
    void * p = ev_zzh_malloc(nmemb*size);
    memset(p, 0, nmemb*size);
    return p;
}



char *ev_zzh_strdup(const char *s)
{
    if(s == NULL)
        return NULL;

    size_t len = strlen(s);
    char *pnew = ( char *)ev_zzh_malloc(len + 1);
    memcpy(pnew, s, len);
    pnew[len] = 0;
    
    return pnew;
}

char *ev_zzh_strndup(const char *s, size_t n)
{
    size_t len = strlen(s);
    len = len <= n ? len : n;
    
    char *pnew = ( char *)ev_zzh_malloc(len + 1);
    memcpy(pnew, s, len);
    pnew[len] = 0;
    return pnew;
}


 
