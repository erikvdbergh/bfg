#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "util.h"
#include "revcomp_func.h"

// reverse a string in place
char *reverse_str(char *str) {
  int len = strlen(str);
  int i;
  for (i = 0; i < len / 2; i++) {
    char c = str[i];
    str[i] = str[len-1 - i];
    str[len-1 - i] = c;
  }
  return str;
}

// return complament of sequence
char *complement(char *seq) {
  int len = strlen(seq);
  int i = 0;
  for (i = 0; i < len; i++) {
    char c = seq[i];
    if (c == 'A') { seq[i] = 'T'; }
    if (c == 'T') { seq[i] = 'A'; }
    if (c == 'C') { seq[i] = 'G'; }
    if (c == 'G') { seq[i] = 'C'; }
  }

  return seq;
}
