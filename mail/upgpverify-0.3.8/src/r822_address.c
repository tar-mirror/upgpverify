#include "r822.h"

#define DEBUGLEVEL 0

struct work {
	stralloc comment;
	stralloc address;
};

static stralloc *
add_rev(stralloc *u, const char *p, unsigned int len)
{
	unsigned int i;
	if (!stralloc_readyplus(u,len)) return 0;
	for (i=len;i;) 
		u->s[u->len+--i]=*p++;
	u->len+=len;
	return u;
}

static int 
finish_comment(stralloc *out, struct work *w)
{
	char *s,*t,*e;
	if (w->comment.s==0 || w->comment.len==0) return 1;
	s=t=e=w->comment.s;
	e+=w->comment.len;
	if (s==e) return 1;
	while (s!=e && *s==' ') s++; /* skip leading spaces */
	while (s!=e) {
		unsigned char c=*s++;
		/* removed continuous spaces and the last space on the line */
		if (c==' ' && (s==e || *s==' ')) continue;
		*t++=c;
	}	
	if (t!=w->comment.s) {
		if (!stralloc_catb(out,"",1)) return 0;
		if (!stralloc_catb(out,w->comment.s,t-w->comment.s)) return 0;
		if (!stralloc_catb(out,"C",1)) return 0;
	}
	w->comment.len=0;
	return 1;
}

static int 
finish_one (stralloc *out, struct work *w)
{
	if (w->address.len && w->address.s) {
		if (!stralloc_catb(out,"",1)) return 0;
		if (!stralloc_catb(out,w->address.s,w->address.len)) return 0;
		if (!stralloc_catb(out,"A",1)) return 0;
	}
	if (!finish_comment(out,w)) return 0;
	w->address.len=0;
	return 1;
}

#if DEBUGLEVEL > 0
#include <stdio.h>
static void
dump(const char *prefix, stralloc *o)
{
	unsigned int l; char *p;
	fputs(prefix,stdout);
	for (l=o->len,p=o->s;l;l--) {
		char c=*p++;
		if (c==0) fputs(" \\0 ",stdout);
		else fputc(c,stdout);
	}
	fputs("\\\n",stdout);
	fflush(stdout);
}
#else
#define dump(y,x) do {(void)x; } while(0)
#endif

int
r822_address(stralloc *out, stralloc *in, int strict)
{
	int er;
	struct work w={STRALLOC_INIT,STRALLOC_INIT};
	char *s;
	char *p;
	int inphrase=0;
	int inrouteaddr=0;
	int done;
	enum {EMPTY,gDPWORD,gDPDOT,gAT,gLPWORD,gLPDOT} state=EMPTY;
	stralloc token=STRALLOC_INIT;
	(void) strict;
	out->len=0;
	er=r822_lex(&token,in,0);
	if (er) {
		stralloc_free(&token);
		return er;
	}
dump("token",&token);
	s=token.s;
	p=token.s+token.len;
	stralloc_ready(&w.address,token.len); /* hint. We'll survive with less */
	stralloc_ready(&w.comment,token.len);
	while (p!=s) {
		char c;
		char *q;
		c=*(--p);
#if DEBUGLEVEL > 1
		fprintf(stdout,"state=%d,%d,%d next token %c\n",state,
			inphrase,inrouteaddr,c);
#endif
		q=p;
		done=0;
		while (p!=s && *p) p--;
		/* tokenlength = q-p-1, token at p+1 */
		if (inphrase) {
			if (c!='S' || 
				(p[1]!='>' && p[1]!=',' && p[1]!=';'))
				goto case_inphrase;
			inphrase=0;
		}
		switch(c) {
		case 'C': 
			if (!add_rev(&w.comment,p+1,q-p-1)) goto err;
			if (!add_rev(&w.comment," ",1)) goto err;
			done=1;
			break;
		case_inphrase:
			if (!add_rev(&w.comment,p+1,q-p-1)) goto err;
			done=1;
			break;
		case 'Q':
		case 'D':
		case 'W':
			if (state==EMPTY || state==gDPDOT || state==gLPDOT || state==gAT
				|| inrouteaddr) {
				if (state==EMPTY || state==gDPDOT)
					state=gDPWORD;
				else 
					state=gLPWORD;
				if (!add_rev(&w.address,p+1,q-p-1)) goto err;
			} else {
				if (strict) goto parse_error;
				finish_one(out,&w);
				if (!add_rev(&w.address,p+1,q-p-1)) goto err;
				state=gDPWORD;
			}
			done=1;
			break;
		case 'S': 
			switch(p[1]) {
			case '@': 
				if (state==gDPWORD || !strict) {
					state=gAT;
					if (!stralloc_catb(&w.address,p+1,1)) goto err;
					done=1;
				} else goto parse_error;
				break;
			case '.': 
				if (state==gLPWORD || state==gDPWORD || !strict) {
					if (state==gLPWORD)
						state=gLPDOT;
					else
						state=gDPDOT;
					if (!stralloc_catb(&w.address,p+1,1)) goto err;
					done=1;
				} else goto parse_error;
				break;
			case '<':
				inphrase=1;
				inrouteaddr=0;
				/* FALL THROUGH */
			case ',': 
				if (inrouteaddr) /* catch broken addresses */
					break;
			case ';':
			case '>': /* route-addr */
				if (p[1]=='>') inrouteaddr=1;
				finish_one(out,&w);
				state=EMPTY;
				done=1;
				break;
			case ':': 
				inphrase=1;
				inrouteaddr=0;
				done=1;
				break;
			}
			break;
		}
		if (!done) {
			if (c!='S') {
				if (!add_rev(&w.address,p+1,q-p-1)) goto err;
			} else if (p[1]!=' ') {
				if (!stralloc_catb(&w.address,p+1,1)) goto err;
			}
		}
	} 
	finish_one(out,&w);
dump("out1: ", out);
	s=out->s;
	p=s+out->len-1;
	while (s<p) {
		char c=*s;
		*s++=*p;
		*p--=c;
	}
dump("out2: ", out);
	stralloc_free(&w.comment);
	stralloc_free(&w.address);
	stralloc_free(&token);
	return R822_OK;
  parse_error:
	stralloc_free(&w.comment);
	stralloc_free(&w.address);
	stralloc_free(&token);
	return R822_ERR_PARSE;
  err:
	stralloc_free(&w.comment);
	stralloc_free(&w.address);
	stralloc_free(&token);
	return R822_ERR_NOMEM;
}
