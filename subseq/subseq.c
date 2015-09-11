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
//  char files[MAX_FILES][MAX_FILENAME_LEN]; //-f --file
  char **files;
  int file_i;
  int quiet; //-q --quiet --silent
  int nomsg; // -s --no-messages
  int begin; //-b --begin
  int end; // -e --end
  int header_coord; // -h --header-coord add subseq coordinates to FASTA header line

  int begingiven;
  int endgiven;
  int filegiven;
};

struct Opts opts;

void opts_init() {
  opts.files = malloc(MAX_FILES * sizeof(char*));
  int i;
  for (i = 0; i < MAX_FILES ; i++) {
    opts.files[i] = malloc(MAX_FILENAME_LEN*sizeof(char));
  }
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

int substr(char *str, int start, int end) {
  if (start > end) {
    int temp = start;
    start = end;
    end = temp;
  }

  int sl = strlen(str);
  if (end - start > sl || end > sl || start > sl) {
    //fprintf(stderr, "substr: desired length longer than string\n");
    return 1;
  }

  int sublen = end - start;
  char tempstr[sublen];
  int temp_i;
  for (temp_i = 0; temp_i < sublen; temp_i++) {
    tempstr[temp_i] = str[start + temp_i];
  }
  tempstr[temp_i] = '\0';
  strcpy(str, tempstr);

  return 0;
}

void printseq(char *seqid, char *seq) {
  if (strcmp(seqid, "") && strcmp(seq, "")) {
  }
}
void print_header(char *seqid) {
  if (opts.header_coord) {
    printf("%s|%i:%i", seqid, opts.begin, opts.end);
  } else {
    printf("%s", seqid);
  }
}

int process_file(FILE *fp) {
  char seqid[2048];

  int insub = 0;
  int nuc_pos = 0;

  int subseq_len = 0;
  int subseq_mem = 65536;

  char line[MAX_LINE_LEN];
  char *subseq = malloc(subseq_mem * sizeof(char));

  while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
    if (line[0] == '>') {
      strcpy(seqid, line);
      strcpy(subseq, "");
      nuc_pos = 1;
    } else {
      int new_nuc_pos = nuc_pos + strlen(line)-1 ;
      if (opts.begin >= nuc_pos && opts.begin < new_nuc_pos) {
        if (opts.end <= new_nuc_pos) { //substr in this one line
          substr(line, opts.begin - nuc_pos, opts.end - nuc_pos);

          print_header(seqid);
          printf("%s\n", line);
        } else {
          insub = 1;
          substr(line, opts.begin - nuc_pos, new_nuc_pos - nuc_pos);

          // does not work if extracted subseq > 2048 (initial seq_mem); not sure if this will be a problem
          subseq_len += strlen(line);
          strcpy(subseq, line);

          nuc_pos = new_nuc_pos;
          continue;
        }
      }

      if (insub) {
        if (opts.end >= nuc_pos && opts.end <= new_nuc_pos) {
          substr(line, 0, opts.end - nuc_pos);

          print_header(seqid);
          printf("%s%s\n", subseq, line);

          insub = 0;
        } else {
          //subseq_mem = cat_subseq(subseq, line, subseq_mem);

          int addlen = strlen(line);
          if (subseq_len + addlen > subseq_mem) {
            while (subseq_mem < subseq_len + addlen) {
              subseq_mem *= 2;
            }
            subseq = realloc(subseq, subseq_mem * sizeof(char));
          }
          subseq_len += addlen;
          strcat(subseq, line);
        }
      }
      nuc_pos = new_nuc_pos;
    }
  }

  free(subseq);
  return 0;
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
    FILE *fp = fopen(opts.files[file_i], "r");
    if (!fp) {
      fprintf(stderr, "Can't open file %s\n", opts.files[file_i]);
    }
    int procret =  process_file(fp);
    if (procret) {
      return 1;
    }

  }

  return 0;
}
