#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "util.h"


FILE *open_fasta(char filename[], int nomsg, int quiet) {
  FILE *fp;

  fp = fopen(filename, "r");

  if (fp == NULL && !nomsg && !quiet) {
    fprintf(stderr, "Can't open input file %s!\n", filename);
  }

  return fp;
}

int initseq(char **seq, int seqsize, int quiet) {
  *seq = calloc(seqsize,(sizeof(char)) );

  if (!seq) {
    if (!quiet) {
      fprintf(stderr, "Could not allocate memory for sequence");
    }
    return 1;
  }

  return 0;
}

int grow_seq(char **seq, int seqsize, int quiet) {
  seqsize *= 2;

  *seq = realloc(*seq, seqsize * sizeof(char));

  if (seq == NULL) {
    if (!quiet) {
      fprintf(stderr, "Could not allocate memory for sequence");
    }
    return 1;
  }

  return 0;
}

int seq_next(FILE *file, char **header, char **sequence, int quiet) {
  char line[MAX_LINE_LEN];
  char *seq;

  int firstline = 1;

  int seqsize = 2048;
  int readsize = 0;


  if (initseq(&seq, seqsize, quiet)) {
    if (!quiet) {
      return 1;
     }
  }

  while (fgets(line, MAX_LINE_LEN, file) != NULL) {
    line[strcspn(line, "\n")] = 0;
    if (firstline) {
      strcpy(*header, line);
      firstline = 0;
      continue;
    }

    int linelen = strlen(line);
    if (readsize + linelen > seqsize) {
      if(grow_seq(&seq, seqsize, quiet)){
        free(seq);
        return 1;
      }
      seqsize *= 2;
    }

    strcat(seq, line);
    readsize += linelen;


    char c = fgetc(file);

    if(c == '>') {
      ungetc(c, file);
      *sequence = seq;
      break;
    }
  }

  return 0;
}
