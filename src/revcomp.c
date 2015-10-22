#include <stdio.h>
#include <getopt.h>

#include "util.h"

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048

struct Opts {
  char file[MAX_FILENAME_LEN]; //-f --file TODO
  int quiet; //-q --quiet --silent TODO
  int nomsg; // -s --no-messages TODO
  int add_suffix; // -a --add-suffix TODO
};

struct Opts opts;

void parseopts(int argc, char *argv[]) {
  struct option longopts[] = {
    {"add-suffix", no_argument, NULL, 'a'},
    {"file", required_argument, NULL, 'f'},
    {"quiet", no_argument, NULL, 'q'},
    {"no-messages", no_argument, NULL 's'},
    { 0, 0, 0, 0}
  }

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
  parseopts(argc, argv);

  return 0;
}
