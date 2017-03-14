#include "r822.h"
#include "byte.h"

/* tokenize a rfc 822 field.
 * takes "in", creates "out".
 * out format:
 * \0token1C\0token2C[...]\0tokenxC
 * where C is one of:
 * C: comment `(some (other))' without the outer ( and )
 * D: domain literal `[....]' including [ and ]
 * Q: quoted string  `"..."' without "
 * S: single character (specials + sp + tab)
 * W: word
 * written after a deep look inside djb's mess822_token.c
 */
#define BUFF_ADD(c) do { \
	*bufp++=c; if (bufp==bufe) { \
			if (!stralloc_catb(out,buf,bufe-buf)) return R822_ERR_NOMEM; \
			bufp=buf; \
	} \
} while(0)
#define BUFF_ADD_MEM(s,l) do { \
	if (bufp+l>=bufe) { \
		if (!stralloc_catb(out,buf,bufp-buf)) return R822_ERR_NOMEM; \
		bufp=buf; \
	} \
	byte_copy(bufp,l,s); \
	bufp+=l; \
} while(0)
#define IS_SPECIAL(c) (specials->len != byte_chr(specials->s,specials->len,c))

int
r822_lex(stralloc *out, stralloc *in, stralloc *specials)
{
	char buf[128];
	char *bufe=buf+sizeof(buf);
	char *bufp=buf;
	char *p, *e;
	if (!specials) {
		static stralloc x;
		if (!x.len) {
			if (!stralloc_copys(&x,R822_DEFAULT_SPECIALS))
				return R822_ERR_NOMEM;
		}
		specials=&x;
	}
	e=in->s+in->len;
	p=in->s;
	out->len=0;
	while (p!=e) {
		char c;
		c=*p++;
		if (c=='(') {
			int level; /* comments */
			BUFF_ADD('\0');
			level=1;
			while (p!=e) {
				c = *p++;
				if (c==')') {
					level--; if (!level) break;
					BUFF_ADD(c);
					continue;
				}
				if (c=='\\' && p!=e) c=*p++;
				else if (c=='(') 
					level++;
				if (c!='\n' && c!='\t')
					BUFF_ADD(c);
				else
					BUFF_ADD(' ');
			}
			BUFF_ADD('C');
		} else if (c=='[') {
			BUFF_ADD_MEM("\0[",2);
			while (p!=e) {
				c = *p++;
				if (c==']') break;
				if (c=='\\' && p!=e) c=*p++;
				if (c!='\n' && c!='\t')
					BUFF_ADD(c);
				else
					BUFF_ADD(' ');
			}
			/* BUFF_ADD_MEM("]D",2); at this place bloats object size */
			BUFF_ADD(']');
			BUFF_ADD('D');
		} else if (c=='"') {
			int found=0;
			BUFF_ADD('\0');
			while (p!=e) {
				c = *p++;
				if (c=='"') { found=1; break; }
				if (c=='\\' && p!=e) c=*p++;
				if (c!='\n' && c!='\t')
					BUFF_ADD(c);
				else
					BUFF_ADD(' ');
			}
			if (!found)
				return R822_ERR_PARSE;
			BUFF_ADD('Q');
		} else if (IS_SPECIAL(c)) {
			BUFF_ADD('\0');
			BUFF_ADD(c);
			BUFF_ADD('S');
		} else if (c==' ' || c=='\r' || c=='\n' || c=='\t' || c=='\0') {
			BUFF_ADD_MEM("\0 S",3);
		} else {
			BUFF_ADD('\0');
			while (1) {
				if (c==' ' || c=='\r' || c=='\n' || c=='\t' || c=='\0')
					break;
				if (c=='\\' && p!=e) c=*p++;
				if (c=='\0')
					c=' ';
				BUFF_ADD(c);
				if (p==e) break;
				c = *p;
				if (IS_SPECIAL(c))
					break;
				p++;
			}
			BUFF_ADD('W');
		} /* big if */
	}
	if (bufp!=buf)
		if (!stralloc_catb(out,buf,bufp-buf)) return R822_ERR_NOMEM;
	return R822_OK;
}

