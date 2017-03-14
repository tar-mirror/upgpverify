/* reimplementation: original was k&r */
#include "case.h"

int
case_starts(const char *ulo, const char *ush)
{
	const unsigned char *lo=ulo;
	const unsigned char *sh=ush;
	unsigned int i;
	if (!case_init_lwrdone) case_init_lwrtab();
	i=0;
	while (1) {
		if (!sh[i]) return 1;
		if (case_lwrtab[lo[i]]!=case_lwrtab[sh[i]])
			return 0;
		i++;
	}
}
