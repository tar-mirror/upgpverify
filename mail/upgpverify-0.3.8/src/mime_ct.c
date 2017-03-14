#include "mime.h"
#include "stralloc.h"
#include "str.h"
#include "r822.h"
#include "bailout.h"

int
mime_content_type_init(stralloc *in, stralloc *store, unsigned int *off, 
	stralloc *res)
{
	int r;
	unsigned int j;
	static stralloc specials;
	if (!specials.len) {
		if (!stralloc_copys(&specials,R822_DEFAULT_SPECIALS)) oom();
		if (!stralloc_append(&specials,"=")) oom();
	}
	r=r822_lex(store,in,&specials);
	if (r!=R822_OK) oom();
	for (j=1;j<store->len;) {
		unsigned int i;
		i=j;
		while (j<store->len && store->s[j]!='\0')
			j++;
		/* \0xW\0 */
		/*   i  j */
		if (j<i+2) return 0;
		if (store->s[j-1]=='W') { /* word */
			if (!stralloc_copyb(res,store->s+i,j-i-1)) oom();
			*off=j+1;
			return 1;
		}
		if (store->s[j-1]!='C') /* comment */
			return 0; /* cannot parse that */
		j++;
	}
	return 0;
}

int
mime_content_type_parse(stralloc *store, unsigned int *off, 
	stralloc *var, stralloc *val)
{
	unsigned int j;
	int phase=0;
	j=*off;
	while (1) {
		unsigned int i=j;
		for (;j<store->len;j++) 
			if (store->s[j]=='\0')
				break;
		if (store->s[j-1]=='C')
			continue;
		if (store->s[j-1]=='S') {
			switch (store->s[j-2]) {
			case ' ':
			case '\n':
			case '\r':
			case '\t':
				j++;
				continue;
			}
		}
		if (j-i<2) return 0;
		if (store->s[j-1]=='C') {
			j++;
		} else if (phase==0) {
			if (j-i!=2) return 0;
			if (store->s[j-1]!='S' || store->s[j-2]!=';') return 0;
			phase=1;
			j++;
		} else if (phase==1) {
			if (store->s[j-1]!='W') return 0;
			if (!stralloc_copyb(var,store->s+i,j-i-1)) oom();
			phase=2;
			j++;
		} else if (phase==2) {
			if (j-i!=2) return 0;
			if (store->s[j-1]!='S' || store->s[j-2]!='=') return 0;
			phase=3;
			j++;
		} else if (phase==3) {
			if (store->s[j-1]!='W' && store->s[j-1]!='Q') return 0;
			if (!stralloc_copyb(val,store->s+i,j-i-1)) oom();
			phase=2;
			j++;
			*off=j;
			return 1;
		}
	}	
}
