#ifndef SEQC_H
#define SEQC_H

#define MAX_FILENAME_LEN 4096
#define MAX_FILES 4096
#define MAX_LINE_LEN 8192

/*
 * Global struct that holds cli options given
 */
struct Opts;

typedef struct OptsTag {
  int c;
  int countnuc;
  int countseq;
  int countlong;
  int sgiven;
  int cgiven;
} Opts;

int digitlen(int i); 

#endif /* SEQC_H */
