#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>

#include "util.h"
#include "sgrep_func.h"

void parseopts(int argc, char *argv[], SgrepOpts *opts) {
  struct option longopts[] = {
    {"regexp", required_argument, NULL, 'e'},
    {"file"  , required_argument, NULL, 'f'},
    {"ignore-case", no_argument, NULL, 'i'},
    {"invert-match", no_argument, NULL, 'v'},
    {"word-regexp", no_argument, NULL, 'w'},
    {"line-regexp", no_argument, NULL, 'x'},
    {"count", no_argument, NULL, 'c'},
    {"color", no_argument, &opts->color, 1},
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
  opts->regex_i = 0;

  while ((c = getopt_long(argc, argv, ":e:f:ivwxcLlm:oqs", longopts, NULL)) != -1) {
    switch(c) {
      case 'e':
        if (opts->regex_i > MAX_REGEXES) {
          if (!opts->nomsg) {
            fprintf(stderr, "A maximum of %i regexes is supported, ignoring %s\n", MAX_REGEXES, optarg);
          }
	} else {
          strcpy(opts->regexes_input[opts->regex_i++], optarg);
	}
        break;
      case 'f':
        strcpy(opts->file, optarg);
        break;
      case 'i':
        opts->ignore_c = 1;
        break;
      case 'v':
        opts->invert_m = 1;
        break;
      case 'w':
        opts->word_m = 1;
        break;
      case 'x':
        opts->line_m = 1;
        break;
      case 'c':
        opts->count = 1;
        break;
      case 'L':
        opts->files_nomatch = 1;
        break;
      case 'l':
        opts->files_match = 1;
        break;
      case 'm':
        opts->max_count = 1;
        break;
      case 'o':
        opts->only_matching = 1;
        break;
      case 'q':
        opts->quiet = 1;
        break;
      case 's':
        opts->nomsg = 1;
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
  SgrepOpts *opts = calloc(1, sizeof(SgrepOpts));
  parseopts(argc, argv, opts);

  if (optind == argc) {
    argv[optind] = "-";
    argc++;
  }

  if (strcmp("",opts->file)) {
    read_regex_file(argc, argv, opts);
  }

  regex_t *regexes = malloc(MAX_REGEXES * sizeof(MAX_REGEXES));
  compile_regexes(regexes, opts);

  while (optind < argc) {
    FILE *file = open_fasta(argv[optind++], 0, 0);
    FastaSeq *seq = newFastaSeq();
    while (!seq_next(file, seq, opts->quiet)) {
      int i;
      for (i = 0; i < opts->regex_i; i++) {
        int reti = regexec(&regexes[i], seq->header, 0, NULL, 0);
        if (!reti) {
          printf("%s\n%s\n",seq->header, seq->seq);
        } else {
          if (opts->invert_m) {
            printf("%s\n%s\n",seq->header, seq->seq);
          }
        }
      }
    }
    regex_t regexes[opts->regex_i];
    compile_regexes(regexes, opts);
  }

  return 0;
}
