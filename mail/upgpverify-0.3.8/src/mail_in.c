#include "stralloc.h"
#include "buffer.h"
#include "getln.h"
#include "bailout.h"
#include "error.h"
#include "open.h"
#include "readwrite.h"
#include "close.h"
#include "fmt.h"
#include "mail_in.h"
#include "str.h"
#include "seek.h"
#include "nowarn.h"

static r822_headhash_t head;
static stralloc  headbuf;

unsigned int
mail_parse_header(int fd, stralloc *start)
{
	stralloc sa=STRALLOC_INIT;
	unsigned int totallen=0;
	int canseek;

	if (-1==seek_begin(fd)) {
		canseek=0;
		if (!start) 
			xbailout(100,errno,"failed to seek in mail",0,0,0);
	} else
		canseek=1;
	if (head.start)	r822_hash_destroy(&head);
	headbuf.len=0;
	/* read header */
	while (1) {
		int gotlf;
		sa.len=0;
		if (-1==getln(buffer_0,&sa,&gotlf,'\n'))
			xbailout(111,errno,"failed to read mail",0,0,0);
		if (!sa.len) break;
		if (!canseek)
			if (!stralloc_cat(start,&sa)) oom();
		totallen+=sa.len;
		if (!stralloc_cat(&headbuf,&sa)) oom();
		/* mail_header needs a \n to overwrite it with \0 */
		if (!gotlf)
			xbailout(111,errno,"unterminated line in header",0,0,0);
		if (sa.len==1) break;
	}
	stralloc_free(&sa);
	if (!canseek) {
		int r;
		r=buffer_feed(buffer_0);
		if (r<0)
			xbailout(111,errno,"failed to read mail",0,0,0);
		if (r>0) {
			char *b;
			b=buffer_peek(buffer_0);
			if (!stralloc_catb(start,b,r)) oom();
			buffer_seek(buffer_0,r);
		}
	}
	if (r822_hash_header(&head,headbuf.s,headbuf.len) != R822_OK)
		oom();
	return totallen;
}

const char *
mail_header(const char *name)
{
	int x;
	const char *value;
	stralloc r=STRALLOC_INIT;
	uo_uint32_t l;
	x=r822_hash_lookup(&head,name,str_len(name),&value,&l);
	if (!x) return 0;
	if (!stralloc_copyb(&r,value,l-1)) oom(); /* cut \n */
	if (!stralloc_0(&r)) oom();
	return r.s;
}
