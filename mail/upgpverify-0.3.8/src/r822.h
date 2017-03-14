#ifndef LIB822_H
#define LIB822_H
#include "stralloc.h"
#include "strhash.h"
#include "uotime.h"

typedef struct {
	strhash hash;
	const char *start;
	unsigned int lines;
	unsigned long bytes;
} r822_headhash_t;

/* 1 found, 0 not */
int r822_hash_lookup(r822_headhash_t *hash,
	const char *name, uo_uint32_t nlen,
    const char **value, uo_uint32_t *vlen);
void r822_hash_lookupstart(r822_headhash_t *hash);
int r822_hash_lookupnext(r822_headhash_t *hash,
	const char *name, uo_uint32_t nlen,
    const char **value, uo_uint32_t *vlen);


#define R822_ERR_NOMEM 1
#define R822_ERR_PARSE 2
#define R822_OK 0

int r822_hash_header(r822_headhash_t *hash,const char *header,uo_uint32_t hlen);
void r822_hash_destroy(r822_headhash_t *hash);
/* convert whatever hash_lookup returned into a string without newlines. */
int r822_canon(stralloc *canon, const char *s, uo_uint32_t llen);

#define R822_DEFAULT_SPECIALS "()<>@,;:\\\".[]"
int r822_lex(stralloc *out, stralloc *in, stralloc *specials);
int r822_address(stralloc *out, stralloc *in, int strict);
int r822_date(uo_sec70_t *, stralloc *in);
int r822_get_newsgroups(stralloc *out, stralloc *in);

#endif
