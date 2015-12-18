#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>

#include "util.h"
#include "subseq_func.h"

int subseq(FastaSeq *seq, int start, int end) {
  if (start > end) {
    int temp = start;
    start = end;
    end = temp;
  }

  int sl = strlen(seq->seq);
  if (end - start > sl || end > sl || start > sl) {
    //fprintf(stderr, "substr: desired length longer than string\n");
    return 1;
  }

  start--;

  int sublen = end - start;

  int i;
  for (i = 0; i < sublen; i++) {
    seq->seq[i] = seq->seq[start + i];
  }
  seq->seq[i] = '\0';
  return 0;
}
