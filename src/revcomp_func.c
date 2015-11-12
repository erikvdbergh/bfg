#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "util.h"
#include "revcomp_func.h"

// initialize the char array that holds the current sequence
int initseq(char **seq, int seqsize, Opts opts) {
  *seq = calloc(seqsize,(sizeof(char)) );

  if (!seq) {
    if (!opts.quiet) {
      fprintf(stderr, "Could not allocate memory for sequence");
    }
    return 1;
  }

  return 0;
}

// double the size of the seq array with realloc
int grow_seq(char **seq, int seqsize, Opts opts) {
  seqsize *= 2;

  *seq = realloc(*seq, seqsize * sizeof(char));

  if (seq == NULL) {
    if (!opts.quiet) {
      fprintf(stderr, "Could not allocate memory for sequence");
    }
    return 1;
  }

  return 0;
}

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


int process_file(FILE *fp, Opts opts) {

  int seqsize = MAX_LINE_LEN;
  int readsize = 0;
  char line[MAX_LINE_LEN];
  char header[MAX_LINE_LEN];
  char *seq = NULL;

  if (initseq(&seq, seqsize, opts)) {
    return 1;
  }

  while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
    line[strcspn(line, "\n")] = 0;

    //fasta header, end of sequence
    if (line[0] == '>') {

      // revcomp and print
      seq = reverse_str(seq);
      seq = complement(seq);
      if (strcmp(header,"") != 0) {
        printf("%s\n%s\n", header, seq);
      }

      // prepare new seq string
      free(seq);
      seqsize = MAX_LINE_LEN;
      if (initseq(&seq, seqsize, opts)) {
        return 1;
      }

      // store header
      strcpy(header, line);

    // sequence, add to current sequence
    } else {
      int linelen = strlen(line);

      // current line will cause seq to overflow, grow it
      if (readsize + linelen > seqsize) {
        if (grow_seq(&seq, seqsize, opts)) {
          return 1;
        }
      }

      // concat read seq to current
      strcat(seq, line);
      readsize += linelen;
    }
  }

  // print final seq
  seq = reverse_str(seq);
  seq = complement(seq);
  if (strcmp(header,"") != 0) {
    printf("%s\n%s\n", header, seq);
  }

  // no memleaks
  free(seq);
  return 0;
}
