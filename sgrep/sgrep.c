// TODO name search only as default - speed optimization
// TODO optional seq search

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048
#define MAX_REGEX_LEN 2048
#define MAX_REGEXES 256
#define MAX_LINE_LEN 2048
#define MAX_SEQ_LEN 4294967296 // 4GB

struct Opts {
  char regexes_input[MAX_REGEXES][MAX_REGEX_LEN]; //-e --regex DONE
  int regex_i;
  char file[MAX_FILENAME_LEN]; //-f --file TODO
  int ignore_c; //-i --ignore-case DONE
  int invert_m; //-v --invert-match DONE
  int word_m; // -w --word-regexp TODO
  int line_m;// -x --line-regexp TODO
  int count; // -c --count TODO
  int color; // --color TODO
  int files_nomatch; // -L --files-without-match TODO
  int files_match; // -l --files-with-matches TODO
  int max_count; // -m --max-count DONE
  int only_matching; // -o --only-matching - print matching part of line only TODO
  int quiet; //-q --quiet --silent TODO
  int nomsg; // -s --no-messages TODO
};

struct Opts opts;

void printmatch(char match[]) {
  printf("%s",match);
}

void parseopts(int argc, char *argv[]) {
  struct option longopts[] = {
    {"regexp", required_argument, NULL, 'e'},
    {"file"  , required_argument, NULL, 'f'},
    {"ignore-case", no_argument, NULL, 'i'},
    {"invert-match", no_argument, NULL, 'v'},
    {"word-regexp", no_argument, NULL, 'w'},
    {"line-regexp", no_argument, NULL, 'x'},
    {"count", no_argument, NULL, 'c'},
    {"color", no_argument, &opts.color, 1},
    {"files-without-match", no_argument, NULL, 'L'},
    {"files-with-matches", no_argument, NULL, 'l'},
    {"max-count", required_argument, NULL, 'm'},
    {"only-matching", no_argument, NULL, 'o'},
    {"quiet", no_argument, NULL, 'q'},
    {"silent", no_argument, NULL, 'q'},
    {"no-messages", no_argument, NULL, 's'},
    { 0, 0, 0, 0} // cargo cult
  };

  char c;
  opts.regex_i = 0;

  while ((c = getopt_long(argc, argv, ":e:f:ivwxcLlm:oqs", longopts, NULL)) != -1) {
    switch(c) {
      case 'e':
        if (opts.regex_i > MAX_REGEXES) {
          fprintf(stderr, "A maximum of %i regexes is supported, ignoring %s\n", MAX_REGEXES, optarg);
	} else {
          strcpy(opts.regexes_input[opts.regex_i++], optarg);
	}
        break;
      case 'f':
        strcpy(opts.file, optarg);
        break;
      case 'i':
        opts.ignore_c = 1;
        break;
      case 'v':
        opts.invert_m = 1;
        break;
      case 'w':
        opts.word_m = 1;
        break;
      case 'x':
        opts.line_m = 1;
        break;
      case 'c':
        opts.count = 1;
        break;
      case 'L':
        opts.files_nomatch = 1;
        break;
      case 'l':
        opts.files_match = 1;
        break;
      case 'm':
        opts.max_count = 1;
        break;
      case 'o':
        opts.only_matching = 1;
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

void compile_regexes(regex_t regexes[]) {
  int i;
  for (i = 0; i < opts.regex_i; i++) {
    int comp_res;
    int cflags = 0;

    if (opts.ignore_c) {
      cflags |= REG_ICASE;
    }

    if (comp_res = regcomp(&regexes[i], opts.regexes_input[i], cflags)) {
      fprintf(stderr, "Invalid expression in regex %s\n", opts.regexes_input[i]);
      exit(EXIT_FAILURE);
    }
  }
}

FILE* open_file(char filename[]) {
  FILE *fp = fopen(filename, "r");

  if (fp == NULL) {
    fprintf(stderr, "Unable to open file\n");
    return NULL;
  }
  return fp;
}

void readfile(FILE *fp, regex_t regexes[]) {
  int seq_size_init = 8388608;
  int seq_size = seq_size_init;
  char *seq_cur = NULL;
  seq_cur = (char *) realloc(seq_cur, seq_size * sizeof(char));

  int matchcount = 0;

  int cur_match = 0;

  char line[MAX_LINE_LEN];

  int lines= 0;

  while (fscanf(fp, "%s", line) != EOF) {
    fprintf(stderr, "reading line %i\n", lines++);
    if (line[0] == '>') {
      if (cur_match) {
        printmatch(seq_cur);
	cur_match = 0;
      }

      seq_cur = (char *) realloc(seq_cur, seq_size_init * sizeof(char));
      seq_size = seq_size_init;
      memset(seq_cur, 0, strlen(seq_cur));
    }

    if (strlen(line) + strlen(seq_cur) > seq_size) { // NEED MOAR MEM
      seq_size *= 2;
      fprintf(stderr, "enlarging mem to %i\n", seq_size);
      seq_cur = (char *) realloc(seq_cur, seq_size * sizeof(char));
    }
    
    seq_cur = strcat(seq_cur, strcat(line,"\n"));

    int i; 
    for (i = 0; i < opts.regex_i; i++) {
      int reti = regexec(&regexes[i], line, 0, NULL, 0);
      if (!reti) {
        matchcount++;
	if (!opts.invert_m) {
	  if (!(opts.max_count != 0 && matchcount > opts.max_count)) {
	    cur_match = 1;
	  }
	}
      } else if (reti == REG_NOMATCH) {
	if (opts.invert_m) {
	  cur_match = 1;
	}
      } else {
        char *msgbuf;
	regerror(reti, &regexes[i], msgbuf, sizeof(msgbuf));
	fprintf(stderr, "Regex match failed: %s\n", msgbuf);
      }
    }
  }
}

int main(int argc, char** argv) {

  parseopts(argc, argv);

  FILE *fp = stdin;
  char filename[MAX_FILENAME_LEN] = "";
  char filenames[MAX_FILENAME_LEN][MAX_FILES];

  while (optind < argc) {
    strcpy(filename, argv[optind++]);
    fp = open_file(filename);
    if (!fp) {
      continue;
    }

    regex_t regexes[opts.regex_i];
    compile_regexes(regexes);

    readfile(fp, regexes);
  }
}
