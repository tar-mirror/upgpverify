/* reimplementaion, GPL */
#include "case.h"

int 
case_diffs(const char *ss,const char *st)
{
	const unsigned char *s=ss;
	const unsigned char *t=st;
	unsigned char x; unsigned char y;
	int i;
	if (!case_init_lwrdone) case_init_lwrtab();

	for (i=0;;i++) {
		x=case_lwrtab[s[i]];
		y=case_lwrtab[t[i]];
		if (x!=y) break;
		if (!x) break;
	}

	return ((int)(unsigned int) x) - ((int)(unsigned int) y);
}
