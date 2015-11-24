#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "util.h"
#include "revcomp_func.h"

// global struct to hold program options

// use gnuopt_long to parse opts
void parseopts(int argc, char *argv[], RevcompOpts opts) {
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

int main(int argc, char** argv) {
  RevcompOpts *opts_ptr = malloc(sizeof(RevcompOpts));
  RevcompOpts opts = *opts_ptr;
  parseopts(argc, argv, opts);

  // no files given, use stdin
  if (optind == argc) {
    argv[optind] = "-";
    argc++;
  }

  FILE *fp;
  FastaSeq *seq = newFastaSeq();

  while (optind < argc) {

    // open stderr or file?
    if (strcmp(argv[optind],"-")) {
      fp = open_fasta(argv[optind], opts.nomsg, opts.quiet);
    } else {
      fp = stdin;
    }

    while(!seq_next(fp, seq, opts.quiet)) {
      reverse_str(seq->seq);
      complement(seq->seq);

      if(opts.add_suffix) {
        strcat(seq->header, "_revcomp");
      }

      printf("%s\n%s\n", seq->header, seq->seq);
      clearFastaSeq(seq);
    }

    optind++;
  }

  deleteFastaSeq(seq);
  free(opts_ptr);

  return 0;
}
