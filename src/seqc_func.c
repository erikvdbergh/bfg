#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "util.h"
#include "seqc_func.h"

/*
 * Quick method to count the amount of digits in a number
 * @param i number to count digits
 * @return amount of digits in number
 */
int digitlen(int i) {
  int len = 0;
  do {
    ++len;
    i /= 10;
  } while (i);

  return len;
}

int countfile(FILE *file, int ***counts, int file_i) {
  FastaSeq *seq = newFastaSeq();

  (*counts)[file_i+1] = calloc(3, sizeof(int));
  if (!(*counts)[file_i+1]) 
    return 1;

  while (!seq_next(file, seq, 0)) {

    int seqlen = strlen(seq->seq);

    if ((*counts)[file_i+1][0] < seqlen)
      (*counts)[file_i+1][0] = seqlen;

    (*counts)[file_i+1][1] += seqlen;
    (*counts)[file_i+1][2]++;
  }
  (*counts)[0][0] += (*counts)[file_i+1][0];
  (*counts)[0][1] += (*counts)[file_i+1][1];
  (*counts)[0][2] += (*counts)[file_i+1][2];

  deleteFastaSeq(seq);
  return 0;
}

void printcounts(int** counts, char** filenames, int file_i, SeqcOpts opts) {

  int widestdigit = 3;
  // if we are printing totals we need those character widths too 
  int i, j;
  i = opts.totals ? 0 : 1;

  for (; i < file_i; i++) {
    for(j = 0; j < 3; j++) {
      if (digitlen(counts[i][j]) > widestdigit)
        widestdigit = digitlen(counts[i][j]);
    }
  }

  for (i = 1; i < file_i+1; i++) {
    printf(" ");
    if (opts.countlong) {
      printf("%*d ", widestdigit, counts[i][0]);
    }
    if (opts.countnuc) {
      printf("%*d ", widestdigit, counts[i][1]);
    }
    if (opts.countseq) {
      printf("%*d ", widestdigit, counts[i][2]);
    }
    printf("%s\n", filenames[i]);
  }  

  if (opts.totals) {

    printf(" ");

    if (opts.countlong) {
      printf("%*d ", widestdigit, counts[0][0]);
    } else {
      if (opts.countnuc) {
        printf("%*d ", widestdigit, counts[0][1]);
      }
      if (opts.countseq) {
        printf("%*d ", widestdigit, counts[0][2]);
      }
    }
    printf("%s\n", filenames[0]);
  }
}
