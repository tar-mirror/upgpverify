#include "stralloc.h"
#include "mime.h"

static unsigned char b64table[256];
static int init_done;

static void
b64init (void)
{
	unsigned int i;
	init_done=1;
	for (i=0;i<256;i++)
		b64table[i]=255;
	for (i = 'A'; i <= 'Z'; i++)
		b64table[i] = i - 'A';
	for (i = 'a'; i <= 'z'; i++)
		b64table[i] = i - 'a' + 26;
	for (i = '0'; i <= '9'; i++)
		b64table[i] = i - '0' + 52;
	b64table['+'] = 62;
	b64table['/'] = 63;
}

int
mime_decode_b64 (stralloc * sa)
{
	long l = 0;					/* keep gcc happy */
	int r;
	int val;
	unsigned int i;
	unsigned int j;
	char *s=sa->s;

	if (!init_done)
		b64init ();

	r=0;
	for (j=0,i=0;i<sa->len;i++) {
		unsigned int c;
		c=s[i];
		if (c=='=') {
			if (r==3) {
				s[j++]= l / 65536; l%=65536;
				s[j++]= l / 256;
			} 
			if (r==2)
				s[j++]= l / 65536;
			r=0;
			break;
		} else 
			val=b64table[c];
		if (val==255)
			continue; /* rfc says ignore unknown characters */
		switch (++r) {
		case 1: l = val * 262144; continue;
		case 2: l += val * 4096; continue;
		case 3: l += val * 64; continue;
		case 4:
			l += val;
			r = 0;
			s[j++]= l / 65536; l%=65536;
			s[j++]= l / 256;
			s[j++]= l % 256;
		}
	}

	if (r) 	
		return 0;
	sa->len=j;
	return 1;
}
