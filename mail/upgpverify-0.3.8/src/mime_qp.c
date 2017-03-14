#include "stralloc.h"
#include "mime.h"

int
mime_decode_qp(stralloc *sa)
{
	unsigned int i;
	unsigned int j;
	char *s=sa->s;
	for (i=j=0;i<sa->len;i++) {
		unsigned char c=s[i];
		unsigned char d;
		switch (c) {
		case '=':
			i++;
			if (i==sa->len) return 0;
			c=s[i];
			if (c=='\n') continue;  /* line break */
			i++;
			if (i==sa->len) return 0;
			d=s[i];
			if (c>='0' && c<='9') 
				c=c-'0';
			else if (c>='A' && c<='F') 
				c=c-'A'+10;
			else if (c>='a' && c<='f') 
				c=c-'a'+10;
			else
				return 0;
			if (d>='0' && d<='9') 
				d=d-'0';
			else if (d>='A' && d<='F') 
				d=d-'A'+10;
			else if (d>='a' && d<='f') 
				d=d-'a'+10;
			else
				return 0;
			s[j++]=(unsigned char) c*16+d;
			break;
		default:
			s[j++]=c;
			break;
		}
	}
	sa->len=j;
	return 1;
}

