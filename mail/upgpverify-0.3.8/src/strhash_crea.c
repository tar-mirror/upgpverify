#include "strhashi.h"
#include "strhash.h"

int
strhash_create(strhash *lv0, unsigned int mod, unsigned int startsize,
	strhash_hashfunc fn)
{
	unsigned int i;
	if (!lv0 || !mod || !fn) { errno=error_noent; return -1; }
	lv0->tab=strhash_alloc(mod*sizeof(strhash_lv0));
	if (!lv0->tab) {errno=error_nomem; return -1; }
	lv0->mod=mod;
	lv0->hashfunc=fn;
	lv0->startsize=startsize;

	for (i=0;i<mod;i++) {
		lv0->tab[i].entries=0;
		lv0->tab[i].count=0;
		lv0->tab[i].space=0;
	}
	return 0;
}

