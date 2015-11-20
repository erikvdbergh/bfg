#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "util.h"
#include "seqc_func.h"

/*
 * Quick method to count the amount of digits in a number
 * @param i number to count digits
 * @return amount of digits in number
 */
int digitlen(int i) {
  int len = 0;
  do {
    ++len;
    i /= 10;
  } while (i);

  return len;
}

int process_file(char *filename, int** counts, int file_i, int tots[4], SeqcOpts opts) {
  FILE *fp;
  if (!strcmp(filename, "-")) {
    fp = stdin; 
  } else {
    fp = open_file(filename, 0, 0);
  }
  if (!fp) {
    return 1;
  }

  char line[MAX_LINE_LEN]; // holds line to read
  int sc = 0; // no. of sequences in this file
  int nuc = 0; // no. of nucleotides in this file
  int curseq = 0; // length of sequence being read
  int longest = 0; // length of longest sequence
  
  while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
    if (line[0] == '>') { // FASTA header, begin new sequence
      sc++;
      longest = curseq > longest ? curseq : longest;
      curseq = 0;
    } else {
      int len = strlen(line);

      //don't count newline
      if (line[len] == '\0') {
        len--;
      }

      nuc    += len;
      curseq += len;
    }
  }

  // let's not forget the last one, like always...
  longest = curseq > longest ? curseq : longest;

  // get max char length of numbers for formatting the final table with totals
  if (opts.countlong) {
    tots[3] = digitlen(longest) <= tots[3] ? tots[3] : digitlen(longest);
  }
  if (opts.countnuc) {
    tots[3] = digitlen(nuc) <= tots[3] ? tots[3] : digitlen(nuc);
  }
  if (opts.countseq) {
    tots[3] = digitlen(sc) <= tots[3] ? tots[3] : digitlen(sc);
  }

  counts[0][file_i] = longest;
  counts[1][file_i] = nuc;
  counts[2][file_i] = sc;

  fclose(fp);

  tots[0] += nuc;
  tots[1] += sc;
  tots[2] = longest > tots[2] ? longest : tots[2];

  return 0;
}

void printcounts(int** counts, int tots[4], char** filenames, int file_i, SeqcOpts opts) {

  // if we are printing totals we need those character widths too 
  if (opts.totals) {
    if (opts.countlong) {
      tots[3] = digitlen(tots[2]) <= tots[3] ? tots[3] : digitlen(tots[2]);
    }
    if (opts.countnuc) {
      tots[3] = digitlen(tots[0]) <= tots[3] ? tots[3] : digitlen(tots[0]);
    }
    if (opts.countseq) {
      tots[3] = digitlen(tots[1]) <= tots[3] ? tots[3] : digitlen(tots[1]);
    }
  }

  int i = 0;
  for (i = 0; i < file_i; i++) {
    printf(" ");
    if (opts.countlong) {
      printf("%*d ", tots[3], counts[0][i]);
    }
    if (opts.countnuc) {
      printf("%*d ", tots[3], counts[1][i]);
    }
    if (opts.countseq) {
      printf("%*d ", tots[3], counts[2][i]);
    }
    printf("%s\n", filenames[i]);
  }  

  if (opts.totals) {

    printf(" ");

    if (opts.countlong) {
      printf("%*d ", tots[3], tots[2]);
      printf("longest\n");
    } else {
      if (opts.countnuc) {
        printf("%*d ", tots[3], tots[0]);
      }
      if (opts.countseq) {
        printf("%*d ", tots[3], tots[1]);
      }
      printf("total\n");
    }
  }
}
