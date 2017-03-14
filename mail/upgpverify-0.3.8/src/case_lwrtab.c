/* GPL */

#include "case.h"
char case_lwrtab[256];
int case_init_lwrdone;

void case_init_lwrtab(void)
{
	unsigned int i;
	for (i=0;i<256;i++) {
		if (i>='A' && i<='Z') case_lwrtab[i]=i-'A'+'a';
		else case_lwrtab[i]=i;
	}
	case_init_lwrdone=1;
}
