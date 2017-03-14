#include "r822.h"
#include "stralloc.h"
#include "alloc.h"
#include "byte.h"
#include "case.h"
#include "scan.h"
#include "str.h"

int 
r822_hash_header(r822_headhash_t *hash,const char *header,uint32 hlen)
{
	uint32 i;
	stralloc hn=STRALLOC_INIT;
	uint32 content=0;
	hash->lines=0;

	if (-1==strhash_create(&hash->hash,1,32,strhash_hash)) 
		return R822_ERR_NOMEM;

	i=0;
	while (i<hlen) {
		uint32 sol=i; /* start of line */
		/* at the start of a line */
		hash->lines++;
		if (header[i]==' ' || header[i]=='\t')  {
			/* continuation */
			while (i<hlen && header[i]!='\n')	
				i++;
			if (i<hlen)
				i++;
			continue;
		}
		while (i<hlen) {
			if (header[i]==':')
				break;
			i++;
		};
		if (content) {
			while (content<sol && (header[content]==' ' || header[content]=='\t'))
				content++;
			if (1!=strhash_enter(&hash->hash,
				1,hn.s,hn.len,0,header+content,sol-content))
				goto nomem;
		}
		if (i==sol) break;
		if (i==hlen) break;
		if (!stralloc_copyb(&hn,header+sol,i-sol))  goto nomem;
		case_lowerb(hn.s,hn.len);
		content=i+1;
		while (i<hlen && header[i]!='\n')	
			i++;
		if (i<hlen) 
			i++;
	}
	stralloc_free(&hn);
	hash->bytes=hlen;
	return R822_OK;
  nomem:
	stralloc_free(&hn);
	strhash_destroy(&hash->hash);
	return R822_ERR_NOMEM;
}

void 
r822_hash_destroy(r822_headhash_t *hash)
{
	strhash_destroy(&hash->hash);
	hash->start=0;
	hash->lines=hash->bytes=0;
}
void r822_hash_lookupstart(r822_headhash_t *hash)
		{ strhash_lookupstart(&hash->hash); }

int r822_hash_lookupnext(r822_headhash_t *hash,
	const char *name, uint32 nlen,
    const char **value, uint32 *vlen)
{
	char *p;
	int got;
	if (!nlen) nlen=str_len(name);
	got=strhash_lookupnext(&hash->hash,name,nlen,&p,vlen);
	if (value)
		*value=p;
	return got;
}

int r822_hash_lookup(r822_headhash_t *hash,const char *h, uint32 hlen,
    const char **value, uint32 *vlen)
{
	char *p;
	int got;
	if (!hlen) hlen=str_len(h);
	got=strhash_lookup(&hash->hash,h,hlen,&p,vlen);
	if (value)
		*value=p;
	return got;
}

