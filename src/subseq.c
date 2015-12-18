#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>

#include "util.h"
#include "subseq_func.h"

int parseopts(int argc, char **argv, SubseqOpts *opts) {
  struct option longopts[] = {
    {"file"  , required_argument, NULL, 'f'},
    {"quiet"  , no_argument, NULL, 'q'},
    {"silent"  , no_argument, NULL, 'q'},
    {"no-messages", no_argument, NULL, 's'},
    {"begin"  , required_argument, NULL, 'b'},
    {"end"  , required_argument, NULL, 'e'},
    {"header-coord"  , no_argument, NULL, 'h'},
    {0, 0, 0, 0}
  };
  char c;
  while ((c = getopt_long(argc, argv, ":qsb:e:h", longopts, NULL)) != -1) {
    switch(c) {
      case 'q':
        opts->quiet = 1;
        break;
      case 's':
        opts->nomsg = 1;
        break;
      case 'b':
        opts->begin = atoi(optarg);
        opts->begingiven = 1;
        break;
       case 'e':
        opts->end = atoi(optarg);
        opts->endgiven = 1;
        break;
       case 'h':
        opts->header_coord = 1;
        break;
       case ':':
        if (!opts->quiet) {
          fprintf(stderr, "%s: Option \'-%c\' requires an argument\n", argv[0], c);
        }
        return 1;
	break;
      case '?':
      default:
        if (!opts->quiet) {
          fprintf(stderr, "%s: unrecognized option \'-%c\'", argv[0], c); 
        }
        return 1;
	break;
    }
  }

  if (opts->begingiven && !opts->endgiven) {
    if (!opts->quiet) {
      fprintf(stderr, "subseq: End coordinates must be specified (-e) if beginning coordinates are given\n");
    }
    return 1;
  }

  if (!opts->begingiven && opts->endgiven) {
    if (!opts->quiet) {
      fprintf(stderr, "subseq: beginning coordinates must be specified (-b) if end coordinates are given\n");
    }
    return 1;
  }

  if (!opts->begingiven && !opts->endgiven) {
    if (argv[optind]) {
      opts->begin = atoi(argv[optind++]);
      opts->end = atoi(argv[optind++]);
    } else {
      if (!opts->quiet) {
        fprintf(stderr, "subseq: Begin and end coordinates need to be specified\n");
      }
      return 1;
    }
  }

  if (opts->begin > opts->end) {
    if (!opts->quiet) {
      fprintf(stderr, "subseq: Begin coordinate is higher than end coordinate, swapping. Try revcomp for reverse complement\n");
    }
    int temp = opts->begin;
    opts->begin = opts->end;
    opts->end = temp;
  }

  return 0;
}

int main(int argc, char** argv) {
  SubseqOpts *opts = calloc(1, sizeof(SubseqOpts));
  if (parseopts(argc, argv, opts)) { // something went wrong, returned 1
    free(opts);
    return 1;
  }

  // no filename arguments left after opt parsing, read from stdin
  if (optind == argc) {
    argv[optind] = "-";
    argc++;
  }

  FastaSeq *seq = newFastaSeq();
  while (optind < argc) {
    FILE *file = open_fasta(argv[optind++], 0, 0);

    while (!seq_next(file, seq, opts->quiet)) {
      subseq(seq, opts->begin, opts->end);

      if (opts->header_coord) {
        printf("%s %i-%i\n%s\n",seq->header, opts->begin, opts->end, seq->seq);
      } else {
        printf("%s\n%s\n",seq->header, seq->seq);
      }
    }

    clearFastaSeq(seq);
    fclose(file);
  }

  deleteFastaSeq(seq);
  free(opts);

  return 0;
}
