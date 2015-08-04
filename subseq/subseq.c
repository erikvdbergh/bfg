#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>

#include "util.h"

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048
#define MAX_LINE_LEN 2048
#define MAX_SEQ_LEN 4294967296 // 4GB

struct Opts {
  char files[MAX_FILES][MAX_FILENAME_LEN]; //-f --file
  int file_i;
  int quiet; //-q --quiet --silent
  int nomsg; // -s --no-messages
  int begin; //-b --begin
  int end; // -e --end

  int begingiven;
  int endgiven;
  int filegiven;
};

void opts_init(struct Opts opts) {
  opts.file_i = 0;
  opts.filegiven = 0;
}

void parseopts(int argc, char **argv, struct Opts opts) {
  struct option longopts[] = {
    {"file"  , required_argument, NULL, 'f'},
    {"quiet"  , no_argument, NULL, 'q'},
    {"silent"  , no_argument, NULL, 'q'},
    {"no-messages", no_argument, NULL, 's'},
    {"begin"  , no_argument, NULL, 'b'},
    {"end"  , no_argument, NULL, 'e'},
  };
  char c;
  while ((c = getopt_long(argc, argv, ":f:qsb:e:", longopts, NULL)) != -1) {
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
      case ':':
        fprintf(stderr, "%s: Option \'-%c\' requires an argument\n", argv[0], optopt);
	break;
      case '?':
      default:
        fprintf(stderr, "%s: unrecognized option \'-%c\'", argv[0], optopt); 
	break;
    }
  }

  if (opts.begingiven && !opts.endgiven) {
    if (!opts.quiet) {
      fprintf(stderr, "end coordinates must be specified (-e) if beginning coordinates are given\n");
    }
  }

  if (!opts.begingiven && opts.endgiven) {
    if (!opts.quiet) {
      fprintf(stderr, "beginning coordinates must be specified (-b) if end coordinates are given\n");
    }
  }

  if (!opts.begingiven && !opts.endgiven) {
    opts.begin = atoi(argv[optind++]);
    opts.end = atoi(argv[optind++]);
  }

  if (!opts.filegiven) {
    while(argv[optind]) {
      strcpy(opts.files[opts.file_i++], argv[optind++]);
    }
  }
}

void process_file(FILE *fp) {

}

int main(int argc, char** argv) {

  struct Opts opts;
  parseopts(argc, argv, opts);

  FILE *fp = stdin;

  int file_i;
  for (file_i = 0; file_i < opts.file_i; file_i++) {
    fp = open_file(opts.files[file_i]);
    process_file(fp);
  }

  return 0;
}
