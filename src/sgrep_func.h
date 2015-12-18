#ifndef SGREP_FUNC_H
#define SGREP_FUNC_H

#include <regex.h>
#include "util.h"

struct SgrepOpts;

typedef struct SgrepOptsTag {
  char regexes_input[MAX_REGEXES][MAX_REGEX_LEN]; //-e --regex DONE
  int regex_i;
  char file[MAX_FILENAME_LEN]; //-f --file DONE
  int ignore_c; //-i --ignore-case DONE
  int invert_m; //-v --invert-match DONE
  int word_m; // -w --word-regexp TODO
  int line_m;// -x --line-regexp TODO
  int count; // -c --count DONE
  int color; // --color TODO
  int files_nomatch; // -L --files-without-match DONE
  int files_match; // -l --files-with-matches DONE
  int max_count; // -m --max-count DONE
  int only_matching; // -o --only-matching - print matching fasta header only DONE
  int quiet; //-q --quiet --silent DONE
  int nomsg; // -s --no-messages DONE
} SgrepOpts;

void printmatch(char match[]);

void compile_regexes(regex_t regexes[], SgrepOpts *opts);

int getfilematch(FILE *fp, regex_t regexes[], SgrepOpts *opts); 

int read_regex_file(int argc, char **argv, SgrepOpts *opts); 

#endif /* SGREP_FUNC_H */
