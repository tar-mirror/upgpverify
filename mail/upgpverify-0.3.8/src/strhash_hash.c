#include "strhashi.h"
#include "strhash.h"

uint32 
strhash_hash(const char *buf, unsigned int len)
{
	uint32 h=0;
	unsigned int o=0;
	while (o<len)
		h=131*h + buf[o++];
	return h;
}
