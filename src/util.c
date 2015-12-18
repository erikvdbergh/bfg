#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "util.h"

#define PROFILE 0
#define DEBUG 0


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
  if (DEBUG)
  fprintf(stderr, "Growing fasta to %i\n", seq->size*2);

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

  if (!strcmp("-", filename)) {
    return stdin;
  }

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

  long profiles[5] = {0,0,0,0,0};
  int lines_done = 0;
  struct timeval start, sofar;

  while (fgets(line, MAX_LINE_LEN, file) != NULL) {
    if (PROFILE) {
      gettimeofday(&start, NULL);
    }
    line[strcspn(line, "\n")] = 0;
    if (firstline) {
      strcpy(seq->header, line);
      firstline = 0;
      continue;
    }

    if (PROFILE) {
      gettimeofday(&sofar, NULL);
      profiles[0] += sofar.tv_usec - start.tv_usec;
      start = sofar;
    }

    int linelen = strlen(line);
    //int linelen = 100;
    if (readsize + linelen > seq->size) {
      if(growFastaSeq(seq, quiet)){
        deleteFastaSeq(seq);
        return 1;
      }
    }

    if (PROFILE) {
      gettimeofday(&sofar, NULL);
      profiles[1] += sofar.tv_usec - start.tv_usec;
      start = sofar;
    }

    int i;
    for(i = 0; i < linelen; i++) {
      seq->seq[readsize++] = line[i];
    }
    seq->seq[readsize] = '\0';

    if (PROFILE) {
      gettimeofday(&sofar, NULL);
      profiles[2] += sofar.tv_usec - start.tv_usec;
      start = sofar;
    }

    char c = fgetc(file);
    ungetc(c, file);
    if(c == '>') {
      break;
    }

    if (PROFILE) {
      gettimeofday(&sofar, NULL);
      profiles[3] += sofar.tv_usec - start.tv_usec;
      start = sofar;
    }

    lines_done++;
    //fprintf(stderr, "lines done: %i\n", lines_done);

    if (lines_done % 1000 == 0 && PROFILE) {
    //if (0) {
      fprintf(stderr, "part1: %li part2: %li  part3: %li  part4: %li\n", profiles[0] / lines_done, profiles[1] / lines_done, profiles[2] / lines_done, profiles[3] / lines_done);
      profiles[0] = 0;
      profiles[1] = 0;
      profiles[2] = 0;
      profiles[3] = 0;
    }
  }

  return 0;
}
