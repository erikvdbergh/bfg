#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "util.h"


FastaSeq *newFastaSeq() {
  FastaSeq *seq = malloc(sizeof(FastaSeq));

  seq->header = malloc(MAX_LINE_LEN * sizeof(char));
  if (seq->header == NULL) {
    return NULL;
  }
  seq->header[0] = '\0';

  seq->seq = malloc(MAX_LINE_LEN * sizeof(char));
  if (seq->seq == NULL) {
    return NULL;
  }
  seq->seq[0] = '\0';

  seq->size = MAX_LINE_LEN;
  return seq;
}

int growFastaSeq(FastaSeq *seq, int quiet) {

  seq->seq = realloc(seq->seq, (seq->size * 2) * sizeof(char));

  if (seq->seq == NULL) {
    if (!quiet) {
      fprintf(stderr, "Could not allocate memory for a");
    }
    return 1;
  }

  seq->size *= 2;

  return 0;
}

void clearFastaSeq(FastaSeq *seq) {
  seq->header[0] = '\0';
  seq->seq[0] = '\0';
}

void deleteFastaSeq(FastaSeq *seq) {
  free(seq->header);
  free(seq->seq);
  free(seq);
}

FILE *open_fasta(char filename[], int nomsg, int quiet) {
  FILE *fp;

  fp = fopen(filename, "r");

  if (fp == NULL && !nomsg && !quiet) {
    fprintf(stderr, "Can't open input file %s!\n", filename);
  }

  return fp;
}


int seq_next(FILE *file, FastaSeq *seq, int quiet) {
  char line[MAX_LINE_LEN];

  int firstline = 1;

  int readsize = 0;

  // check for end of file
  char c = fgetc(file);
  if(c == EOF) {
    return 1;
  } else {
    ungetc(c, file);
  }

  while (fgets(line, MAX_LINE_LEN, file) != NULL) {
    line[strcspn(line, "\n")] = 0;
    if (firstline) {
      strcpy(seq->header, line);
      firstline = 0;
      continue;
    }

    int linelen = strlen(line);
    if (readsize + linelen > seq->size) {
      if(growFastaSeq(seq, quiet)){
        deleteFastaSeq(seq);
        return 1;
      }
    }

    strcat(seq->seq, line);
    readsize += linelen;

    char c = fgetc(file);
    ungetc(c, file);
    if(c == '>') {
      break;
    }
  }

  return 0;
}
