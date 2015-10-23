#ifndef REVCOMP_H
#define REVCOMP_H

#include <stdlib.h>
#include <stdio.h>

void parseopts(int argc, char *argv[]);

int initseq(char **seq, int seqsize);

int grow_seq(char **seq, int seqsize);

char *reverse_str(char *str);

char *complement(char *seq);

int process_file(FILE *fp);

#endif /* REVCOMP_H */
