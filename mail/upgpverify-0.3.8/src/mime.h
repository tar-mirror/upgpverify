#ifndef MIME_H
#define MIME_H
#include "stralloc.h"

int mime_content_type_init(stralloc *in, stralloc *store, unsigned int *off, 
	stralloc *res);
int mime_content_type_parse(stralloc *store, unsigned int *off, 
	stralloc *var, stralloc *val);
int mime_decode_qp(stralloc *sa);
int mime_decode_b64(stralloc *sa);

#endif
