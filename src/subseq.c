#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>

#include "util.h"
#include "subseq_func.h"

void opts_init(Opts opts) {
  opts.files = malloc(MAX_FILES * sizeof(char*));
  int i;
  for (i = 0; i < MAX_FILES ; i++) {
    opts.files[i] = malloc(MAX_FILENAME_LEN*sizeof(char));
  }
  opts.file_i = 0;
  opts.filegiven = 0;
}

int parseopts(int argc, char **argv, Opts opts) {
  struct option longopts[] = {
    {"file"  , required_argument, NULL, 'f'},
    {"quiet"  , no_argument, NULL, 'q'},
    {"silent"  , no_argument, NULL, 'q'},
    {"no-messages", no_argument, NULL, 's'},
    {"begin"  , required_argument, NULL, 'b'},
    {"end"  , required_argument, NULL, 'e'},
    {"header-coord"  , required_argument, NULL, 'h'},
    {0, 0, 0, 0}
  };
  char c;
  while ((c = getopt_long(argc, argv, ":f:qsb:e:h", longopts, NULL)) != -1) {
    switch(c) {
      case 'f':
        strcpy(opts.files[opts.file_i++], optarg);
        opts.filegiven = 1;
        break;
      case 'q':
        opts.quiet = 1;
        break;
      case 's':
        opts.nomsg = 1;
        break;
      case 'b':
        opts.begin = atoi(optarg);
        opts.begingiven = 1;
        break;
       case 'e':
        opts.end = atoi(optarg);
        opts.endgiven = 1;
        break;
       case 'h':
        opts.header_coord = 1;
       case ':':
        if (!opts.quiet) {
          fprintf(stderr, "%s: Option \'-%c\' requires an argument\n", argv[0], optopt);
        }
        return 1;
	break;
      case '?':
      default:
        if (!opts.quiet) {
          fprintf(stderr, "%s: unrecognized option \'-%c\'", argv[0], optopt); 
        }
        return 1;
	break;
    }
  }

  if (opts.begingiven && !opts.endgiven) {
    if (!opts.quiet) {
      fprintf(stderr, "subseq: End coordinates must be specified (-e) if beginning coordinates are given\n");
    }
    return 1;
  }

  if (!opts.begingiven && opts.endgiven) {
    if (!opts.quiet) {
      fprintf(stderr, "subseq: beginning coordinates must be specified (-b) if end coordinates are given\n");
    }
    return 1;
  }

  if (!opts.begingiven && !opts.endgiven) {
    if (argv[optind]) {
      opts.begin = atoi(argv[optind++]);
      opts.end = atoi(argv[optind++]);
    } else {
      if (!opts.quiet) {
        fprintf(stderr, "subseq: Begin and end coordinates need to be specified\n");
      }
      return 1;
    }
  }

  if (opts.begin > opts.end) {
    if (!opts.quiet) {
      fprintf(stderr, "subseq: Begin coordinate is higher than end coordinate, swapping. Try revcomp for reverse complement\n");
    }
    int temp = opts.begin;
    opts.begin = opts.end;
    opts.end = temp;
  }

  if (!opts.filegiven) {
    while(argv[optind]) {
      strcpy(opts.files[opts.file_i++], argv[optind++]);
    }
  }

  return 0;
}

int main(int argc, char** argv) {
  Opts opts;
  opts_init(opts);
  if (parseopts(argc, argv, opts)) { // something went wrong, returned 1
    return 1;
  }

  if (!opts.file_i) {
    if (process_file(stdin, opts)) {
      return 1;
    }
  }

  int file_i;
  for (file_i = 0; file_i < opts.file_i; file_i++) {
    FILE *fp = fopen(opts.files[file_i], "r");
    if (!fp) {
      fprintf(stderr, "Can't open file %s\n", opts.files[file_i]);
    }
    int procret = process_file(fp, opts);
    if (procret) {
      return 1;
    }

  }

  return 0;
}
