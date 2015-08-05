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

struct Opts opts;

void opts_init() {
  opts.file_i = 0;
  opts.filegiven = 0;
}

int parseopts(int argc, char **argv) {
  struct option longopts[] = {
    {"file"  , required_argument, NULL, 'f'},
    {"quiet"  , no_argument, NULL, 'q'},
    {"silent"  , no_argument, NULL, 'q'},
    {"no-messages", no_argument, NULL, 's'},
    {"begin"  , required_argument, NULL, 'b'},
    {"end"  , required_argument, NULL, 'e'},
    {0, 0, 0, 0}
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
      fprintf(stderr, "subseq: Begin coordinate is higher than end coordinate, swapping. Try revcomp for reverse complement\n";
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

int process_file(FILE *fp) {
  int nuc_pos = 0;
  int insub = 0;
  char subseq[MAX_SEQ_LEN];

  while (fscanf(fp, "%s", line) != EOF) {
    if (line[0] == '>') {
      nuc_pos = 0;
    } else {
      if (!insub) {
        linelen = strlen(line);
        if (nuc_pos + linelen > opts.begin && nuc_pos + linelen > opts.end) { // subseq in one line
          if (line = substr(line, opts.begin - nuc_pos, opts.end - nuc_pos)) {
            strcpy(subseq, line);
          } else {
            //substr error
          }
        } else if (nuc_pos + linelen > opts.begin) {
          if (line = substr(line, opts.begin - nuc_pos, linelen)) {
          strcpy(subseq, line);
          } else {
            //substr error
          }
          insub = 1;
          continue;
        }
      } else {
        linelen = strlen(line);
        if (nuc_pos + linelen > opts.end) {
          if (line = substr(line, nuc_pos, opts.end - nuc_pos)) {
            strcpy(subseq, strcat(subseq, line);
          } else {
            // substr error
          }
        }
      }
    }
  }
}

char *substr(char *str, int start, int end) {
  if (start > end) {
    int temp = start;
    start = end;
    end = temp;
  }

  int sl = strlen(str)
  if (end - start > sl || end > sl || start > sl) {
    fprintf(stderr, "substr: desired length longer than string\n";
    return NULL;
  }

  int sublen = end - start;
  char tempstr[sublen];
  int temp_i;
  for (temp_i = 0; temp_i < sublen; temp_i++) {
    tempstr[temp_i] = str[start + temp_i];
  }
  tempstr[temp_i] = '\0';
  strcpy(str, tempstr);

  return str;
}

int main(int argc, char** argv) {

  opts_init();
  if (parseopts(argc, argv)) { // something went wrong, returned 1
    return 1;
  }

  if (!opts.file_i) {
    if (process_file(stdin)) {
      return 1;
    }
  }

  int file_i;
  for (file_i = 0; file_i < opts.file_i; file_i++) {
    FILE *fp = open_file(opts.files[file_i]);
    if (process_file(fp)) {
      return 1;
    }
  }

  return 0;
}
