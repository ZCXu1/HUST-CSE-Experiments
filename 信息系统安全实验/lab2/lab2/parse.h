#ifndef PARSE_H
#define PARSE_H

#include "http-tree.h"

void *Parse_parse(int fd, int reqOnly);
Body_t *getBody(int *n);
#endif
