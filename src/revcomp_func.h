#ifndef REVCOMP_H
#define REVCOMP_H

#include <stdlib.h>
#include <stdio.h>

#include "util.h"

struct RevcompOptsTag ;

typedef struct RevcompOptsTag {
  char file[MAX_FILENAME_LEN]; //-f --file TODO
  int quiet; //-q --quiet --silent TODO
  int nomsg; // -s --no-messages TODO
  int add_suffix; // -a --add-suffix TODO
} RevcompOpts;

char *reverse_str(char *str);

char *complement(char *seq);

#endif /* REVCOMP_H */
