#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "revcomp.h"

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048
#define MAX_LINE_LEN 2048

// global struct to hold program options
struct Opts {
  char file[MAX_FILENAME_LEN]; //-f --file TODO
  int quiet; //-q --quiet --silent TODO
  int nomsg; // -s --no-messages TODO
  int add_suffix; // -a --add-suffix TODO
};

struct Opts opts;

// use gnuopt_long to parse opts
void parseopts(int argc, char *argv[]) {
  struct option longopts[] = {
    {"add-suffix", no_argument, NULL, 'a'},
    {"file", required_argument, NULL, 'f'},
    {"quiet", no_argument, NULL, 'q'},
    {"no-messages", no_argument, NULL, 's'},
    { 0, 0, 0, 0}
  };

  char c;

  while ((c = getopt_long(argc, argv, ":af:qs", longopts, NULL)) != -1) {
    switch(c) {
      case 'a':
        opts.add_suffix = 1;
        break;
      case 'f':
        strcpy(opts.file, optarg);
        break;
      case 'q':
        opts.quiet = 1;
        break;
      case 's':
        opts.nomsg = 1;
        break;
      case ':':
        fprintf(stderr, "%s: Option \'-%c\' requires an argument\n", argv[0], optopt);
        break;
      case '?':
      default:
        fprintf(stderr, "%s: unrecognized option \'-%c\'", argv[0], optopt);
        break;
    }
  }
}

// initialize the char array that holds the current sequence
int initseq(char **seq, int seqsize) {
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
int grow_seq(char **seq, int seqsize) {
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


int process_file(FILE *fp) {

  int seqsize = MAX_LINE_LEN;
  int readsize = 0;
  char line[MAX_LINE_LEN];
  char header[MAX_LINE_LEN];
  char *seq = NULL;

  if (initseq(&seq, seqsize)) {
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
      if (initseq(&seq, seqsize)) {
        return 1;
      }

      // store header
      strcpy(header, line);

    // sequence, add to current sequence
    } else {
      int linelen = strlen(line);

      // current line will cause seq to overflow, grow it
      if (readsize + linelen > seqsize) {
        if (grow_seq(&seq, seqsize)) {
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

int main(int argc, char** argv) {
  parseopts(argc, argv);

  // no files given, use stdin
  if (optind == argc) {
    argv[optind] = "-";
    argc++;
  }

  FILE *fp;

  while (optind < argc) {

    // open stderr or file?
    if (strcmp(argv[optind],"-")) {
      fp = fopen(argv[optind], "r");
    } else {
      fp = stdin;
    }

    // something went wrong opening file
    if (fp == NULL) {
      if (!opts.nomsg) {
        fprintf(stderr, "Unable to open file %s\n", argv[optind]);
      }
      return 1;
    }

    if (!process_file(fp)) {
      return 1;
    }

    optind++;
  }

  return 0;
}
