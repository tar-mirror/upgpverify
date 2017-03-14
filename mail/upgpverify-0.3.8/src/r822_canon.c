#include "r822.h"
int
r822_canon(stralloc *canon, const char *s, uint32 llen)
{
	uint32 i,j;
	/* +1 in case no \n, need \0 */
	if (!stralloc_ready(canon,llen+1)) return R822_ERR_NOMEM;
	i=j=0;
	/* skip initial white space */
	while (i<llen) {
		if (s[i]!=' ' && s[i]!='\t' && s[i]!='\n')
			break;
		i++;
	}
	while (i<llen) {
		if (s[i]!='\n') {
			canon->s[j++]=s[i++];
			continue;
		}
		while (s[i]==' ' || s[i]=='\t' || s[i]=='\n' || s[i]=='\r')
			if (++i==llen)
				break;
		if (i!=llen)
			canon->s[j++]=' ';
	}
	/* remove trailing white space */
	while (j && (canon->s[j-1]==' ' || canon->s[j-1]=='\t'))
		j--;
	canon->s[j++]=0;
	canon->len=j;
	return R822_OK;
}
