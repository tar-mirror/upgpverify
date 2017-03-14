/* reimplementation, GPL */
#include "case.h"

void case_lowerb(char *ss,unsigned int len)
{
	unsigned char *s=ss;
	unsigned int i;
    if (!case_init_lwrdone) case_init_lwrtab();
	for(i=0;i!=len;i++) 
		s[i]=case_lwrtab[s[i]];
}
