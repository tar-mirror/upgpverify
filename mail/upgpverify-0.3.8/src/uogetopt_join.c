#include <stdlib.h> /* malloc */
#include "uogetopt.h"

#define L longname
#define S shortname

static unsigned int 
count(uogetopt_t *opts)
{
	unsigned int c=0;
	unsigned int i;
	for (i=0;opts[i].S || opts[i].L;i++) {
		if ((opts[i].argtype & ~(UOGO_HIDDEN))==UOGO_INCLUDE) {
			if (opts[i].var)
				c+=count((uogetopt_t *)opts[i].var);
		} else
			c++;
	}
	return c;
}
static unsigned int
copy(uogetopt_t *out, uogetopt_t *in, int off, int lv)
{
	unsigned int i;
	for (i=0;in[i].S || in[i].L;i++) {
		if ((in[i].argtype & ~(UOGO_HIDDEN))==UOGO_INCLUDE) {
			if (in[i].var)
				off=copy(out,(uogetopt_t *)in[i].var,off,lv+1);
		} else 
			out[off++]=in[i];
	}
	if (lv==0)
		out[off]=in[i];
	return off;
}

uogetopt_t *
uogetopt_join(uogetopt_t *opts)
{
	uogetopt_t *o;
	unsigned int n;
	n=count(opts)+1; /* null */
	o=malloc(sizeof(uogetopt_t)*n);
	if (!o) return 0;
	copy(o,opts,0,0);
	return o;
}
void
uogetopt_free(uogetopt_t *opts)
{ 	
	free(opts);
}
