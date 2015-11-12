#ifndef REVCOMP_H
#define REVCOMP_H

#include <stdlib.h>
#include <stdio.h>

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048
#define MAX_LINE_LEN 2048

struct OptsTag ;

typedef struct OptsTag {
  char file[MAX_FILENAME_LEN]; //-f --file TODO
  int quiet; //-q --quiet --silent TODO
  int nomsg; // -s --no-messages TODO
  int add_suffix; // -a --add-suffix TODO
} Opts;

int initseq(char **seq, int seqsize, Opts opts);

int grow_seq(char **seq, int seqsize, Opts opts);

char *reverse_str(char *str);

char *complement(char *seq);

int process_file(FILE *fp, Opts opts);

#endif /* REVCOMP_H */
