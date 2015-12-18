// TODO FIx not reading from stdin
// TODO file open message noto working correctly

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>

#include "util.h"
#include "sgrep_func.h"

void printmatch(char match[]) {
  printf("%s",match);
}

void compile_regexes(regex_t regexes[], SgrepOpts *opts) {
  int i;
  for (i = 0; i < opts->regex_i; i++) {
    int comp_res;
    int cflags = 0;

    if (opts->ignore_c) {
      cflags |= REG_ICASE;
    }

    if ( (comp_res = regcomp(&regexes[i], opts->regexes_input[i], cflags)) ) {
      if (!opts->nomsg) {
        fprintf(stderr, "Invalid expression in regex %s\n", opts->regexes_input[i]);
      }
      exit(EXIT_FAILURE);
    }
  }
}

// Note that the returned int does NOT indicate failure or success!
// 1 is file matched, 0 is no match
int getfilematch(FILE *fp, regex_t regexes[], SgrepOpts *opts) {
  int matchcount = 0;

  int cur_match = 0;

  char line[MAX_LINE_LEN];

  //int lines= 0;

  while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
    if (line[0] == '>') {
      cur_match = 0;
      int i;
      for (i = 0; i < opts->regex_i; i++) {
        int reti = regexec(&regexes[i], line, 0, NULL, 0);
        if (!reti) {
          matchcount++;
          if (!opts->invert_m) {
            if (!(opts->max_count != 0 && matchcount > opts->max_count)) {
              cur_match = 1;
              if (!opts->quiet && !opts->count && !opts->files_match && !opts->files_nomatch) {
                printf("%s", line);
              }

              if (opts->files_match || opts->files_nomatch) {
                return 1;
              }
            }
          }
        } else if (reti == REG_NOMATCH) {
          if (opts->invert_m) {
            cur_match = 1;
          }
        } else {
          char msgbuf[4096];
          regerror(reti, &regexes[i], msgbuf, sizeof(msgbuf));
          if (!opts->nomsg) {
            fprintf(stderr, "Regex match failed: %s\n", msgbuf);
          }
        }
      }
    } else {
      if (cur_match) {
        if (!opts->quiet && !opts->count && !opts->only_matching) {
          printf("%s", line);
        }
      }
    }
  }

  if (opts->count && !opts->quiet && !opts->files_match && !opts->files_nomatch) {
    printf("%i\n", matchcount);
  }

  return 0;
}

int read_regex_file(int argc, char **argv, SgrepOpts *opts) {
  FILE *fp = open_fasta(opts->file, opts->nomsg, opts->quiet);
  if (!fp) {
    return 1;
  }

  char line[MAX_LINE_LEN];
  while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
    // remove trailing newline, thanks to Tim Čas from StackOverflow 
    // (http://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input)
    line[strcspn(line, "\n")] = 0;
    if (opts->regex_i == MAX_REGEXES) {
      if (!opts->quiet && !opts->nomsg) {
        fprintf(stderr, "Too many regexes, ignoring %s\n", line);
        fprintf(stderr, "This version of sgrep was compiled with MAX_REGEXES: %i", MAX_REGEXES);
      }
    } else {
      strcpy(opts->regexes_input[opts->regex_i++], line);
    } 
  }

  return 0;
}
