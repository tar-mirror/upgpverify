#include "strhashi.h"

void 
strhash_get(struct strhash_entry *e, char **key, char **data)
{
	if (key) {
		if (strhash_allocated(e->keylen))
			*key=(char *)&e[1];
		else
			*key=*(char **)&e[1];
	}
	if (data) {
		if (strhash_allocated(e->datalen)) {
			if (strhash_reallen(e->datalen) > sizeof(void *))
				*data=e->u.dataptr;
			else
				*data=e->u.data;
		} else
			*data=e->u.dataptr;
	}
}

