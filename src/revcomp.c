#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048
#define MAX_LINE_LEN 2048

struct Opts {
  char file[MAX_FILENAME_LEN]; //-f --file TODO
  int quiet; //-q --quiet --silent TODO
  int nomsg; // -s --no-messages TODO
  int add_suffix; // -a --add-suffix TODO
};

struct Opts opts;

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

int print_revcomp(char *header, char *seq) {
  int len = strlen(seq);
  len--;
  int i = 0;

  // reverse seq (swap in-place)
  for (i = 0; i < len / 2; i++) {
    char c = seq[i];
    seq[i] = seq[len - i];
    seq[len - i] = c;
  }

  //translate char
  for (i = 0; i < len; i++) {
    char c = seq[i];
    if (c == 'A') { seq[i] = 'T'; }
    if (c == 'T') { seq[i] = 'A'; }
    if (c == 'C') { seq[i] = 'G'; }
    if (c == 'G') { seq[i] = 'C'; }
  }

  if (strcmp(header,"")) {
    printf("%s\n%s\n", header, seq);
  }

  return 0;
}

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

int process_file(FILE *fp) {
  char line[MAX_LINE_LEN];
  char header[MAX_LINE_LEN];
  char *seq = NULL;
  int seqsize = MAX_LINE_LEN;
  int readsize = 0;

  if (initseq(&seq, seqsize)) {
    return 1;
  }

  while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
    line[strcspn(line, "\n")] = 0;
    if (line[0] == '>') {
      print_revcomp(header, seq);
      free(seq);
      seqsize = MAX_LINE_LEN;
      if (initseq(&seq, seqsize)) {
        return 1;
      }

      strcpy(header, line);
    } else {
      int linelen = strlen(line);
      if (readsize + linelen > seqsize) {
        if (grow_seq(&seq, seqsize)) {
          return 1;
        }
      }
      strcat(seq, line);
      readsize += linelen;
    }
  }

  print_revcomp(header, seq);
  free(seq);
  return 0;
}

int main(int argc, char** argv) {
  parseopts(argc, argv);

  if (optind == argc) {
    argv[optind] = "-";
    argc++;
  }

  FILE *fp;

  while (optind < argc) {
    if (strcmp(argv[optind],"-")) {
      fp = fopen(argv[optind], "r");
    } else {
      fp = stdin;
    }

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
