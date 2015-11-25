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
  opts->countnuc = 1;
  opts->countseq = 1;

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
  SeqcOpts *opts_ptr = calloc(1, sizeof(SeqcOpts));
  SeqcOpts opts = *opts_ptr;

  parseopts(argc, argv, &opts);

  // print totals if more than 1 file
  opts.totals = argc - optind > 1 ? 1 : 0;

  // no filename arguments left after opt parsing, read from stdin
  if (optind == argc) {
    argv[optind] = "-";
    argc++;
  }


  // counts structure as follows:
  // 0	totallong	totalnuc	totalseq
  // 1  file1long	file1nuc	file1seq
  // 2  file2seq	file2nuc	file2seq
  // ...etc...
  // filenames has filenames at same row index (or "total" at i = 0);
  // 
  int **counts = malloc((MAX_FILES+1) * sizeof(int*));
  if (!counts)
    return 1;

  counts[0] = calloc(3, sizeof(int));

  char **filenames = malloc(MAX_FILES * sizeof(char*));
  if (!filenames)
    return 1;

  if (opts.countlong) {
    filenames[0] = "longest";
  } else {
    filenames[0] = "total";
  }

  int file_i = 0;

  while (optind < argc && file_i < MAX_FILES-1) {
    // store filename
    filenames[file_i+1] = malloc(MAX_FILENAME_LEN * sizeof(char));
    strcpy(filenames[file_i+1], argv[optind]);

    FILE *file = open_fasta(argv[optind++], 0, 0);

    countfile(file, &counts, file_i++);
    fclose(file);
  } 

  // print the whole lot
  printcounts(counts, filenames, file_i, opts);

  if (file_i > MAX_FILES-2) {
    fprintf(stderr, "seqc supports up to %i files, rest is ignored. Recompile with MAX_FILES=x if you need more\n", MAX_FILES);
  }

  // cleanup
  int i = 0;
  for (i = 0; i < file_i+1; i++) {
    free(counts[i]);
  }
  free(counts);

  for (i = 1; i < file_i+1; i++) {
    free(filenames[i]);
  }
  free(filenames);
  free(opts_ptr);

  return 0;
}
