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
  int countnuc;
  int countseq;
  int countlong;
  int sgiven;
  int cgiven;
  int totals;
} Opts;

int digitlen(int i); 

int process_file(char *filename, int** counts, int file_i, int tots[4], Opts opts);

void printcounts(int** counts, int tots[4], char** filenames, int file_i, Opts opts);

#endif /* SEQC_H */
