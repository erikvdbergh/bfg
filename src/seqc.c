#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "seqc_func.h"

/*
 * Parse cli options using GNU getopt
 * @param argc: cli argument count
 * @param argv: cli arguments
 * @return: 0 on success
 */ 
int parseopts(int argc, char **argv, SeqcOpts *opts) {
  char c;

  // by default count sequences and nucleotides
  opts->sgiven = 0;
  opts->cgiven = 0;
  opts->totals = 0;
  opts->countnuc = 1;
  opts->countseq = 1;
  opts->countlong = 0;

  struct option longopts[] = {
    {"characters", no_argument, NULL, 'c'},
    {"sequences", no_argument, NULL, 's'},
    {"max-sequence-length", no_argument, NULL, 'L'},
    {0, 0, 0, 0}
  };

  while ((c = getopt_long(argc, argv, ":csL", longopts, NULL)) != -1) {
    switch(c) {
    case 'c':
      opts->countnuc = 1;
      opts->cgiven = 1;
      if (!opts->sgiven) {
        opts->countseq = 0;
      }
      break;
    case 's':
      opts->countseq = 1;
      opts->sgiven = 1;
      if (!opts->cgiven) {
        opts->countnuc = 0;
      }
      break;
    case 'L':
      opts->countlong = 1;
      if (!opts->sgiven) {
        opts->countseq = 0;
      }
      if (!opts->cgiven) {
        opts->countnuc = 0;
      }
      break;
    case '?':
      fprintf(stderr,
              "Unrecognized option: -%c\n", optopt);
    }
  }

  return 0;
}

int main(int argc, char** argv) {
  SeqcOpts opts;

  parseopts(argc, argv, &opts);

  //char filename[256] = "";

  // print totals if more than 1 file
  opts.totals = argc - optind > 1 ? 1 : 0;

  // no filename arguments left after opt parsing, read from stdin
  if (optind == argc) {
    argv[optind] = "-";
    argc++;
  }

  // hold totals across files
  // tots[0] = nucleotides
  // tots[1] = sequences
  // tots[2] = longest seq
  // tots[3] = character width of highest count (for print alignment)
  int tots[4];

  tots[3] = 3; // default width of table column

  int **counts = malloc(3 * sizeof(int*));
  if (counts) {
    int i = 0;
    for (i = 0; i < 3; i++) {
      counts[i] = malloc(MAX_FILES * sizeof(int));
    }
  }

  char **filenames = malloc(MAX_FILES * sizeof(char*));
  if (filenames) {
    int i = 0;
    for (i = 0; i < MAX_FILES; i++) {
      filenames[i] = malloc(MAX_FILENAME_LEN * sizeof(char));
    }
  }

  int file_i = 0;

  while (optind < argc && file_i < MAX_FILES-1) {
    process_file(argv[optind++], counts, file_i, tots, opts);
    strcpy(filenames[file_i++],argv[optind-1]);
  } 

  // print the whole lot
  printcounts(counts, tots, filenames, file_i, opts);

  if (file_i > MAX_FILES-2) {
    fprintf(stderr, "seqc supports up to %i files, rest is ignored. Recompile with MAX_FILES=x if you need more\n", MAX_FILES);
  }

  // cleanup
  int i = 0;
  for (i = 0; i < 3; i++) {
    free(counts[i]);
  }
  free(counts);
  for (i = 0; i < MAX_FILES; i++) {
    free(filenames[i]);
  }
  free(filenames);

  return 0;
}
