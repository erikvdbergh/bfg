#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048
#define MAX_REGEX_LEN 2048
#define MAX_REGEXES 256
#define MAX_LINE_LEN 2048
#define SEQ_INIT 33554432
#define MAX_SEQ_LEN 4294967296 // 4GB

struct FastaSeq;

typedef struct FastaSeqtag {
  char *header;
  char *seq;
  int size;
} FastaSeq;

FastaSeq *newFastaSeq();

int growFastaSeq(FastaSeq *seq, int quiet);

void clearFastaSeq(FastaSeq *seq);

void deleteFastaSeq(FastaSeq *seq);

FILE *open_fasta(char filename[], int nomsg, int quiet);

int seq_next(FILE *file, FastaSeq *seq, int quiet);

#endif /* UTIL_H */
