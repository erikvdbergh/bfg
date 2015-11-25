#ifndef SEQC_H
#define SEQC_H

#include "util.h"

/*
 * Global struct that holds cli options given
 */
struct SeqcOpts;

typedef struct SeqcOptsTag {
  int countnuc;
  int countseq;
  int countlong;
  int sgiven;
  int cgiven;
  int totals;
} SeqcOpts;

int digitlen(int i); 

int countfile(FILE *file, int ***counts, int file_i);

void printcounts(int** counts, char** filenames, int file_i, SeqcOpts opts);

#endif /* SEQC_H */
