// TODO fix prgram not reading from stdin
// TODO use longopts
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048
#define MAX_LINE_LEN 8192

/*
 * Global struct that holds cli options given
 */
struct Opts {
  int c;
  int countnuc;
  int countseq;
  int countlong;
  int sgiven;
  int cgiven;
};

struct Opts opts;

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

/*
 * Parse cli options using GNU getopt
 * @param argc: cli argument count
 * @param argv: cli arguments
 * @return: 0 on success
 */ 
int parseopts(int argc, char **argv) {
  char c;

  // by default counts sequences and nucleotides
  opts.countnuc = 1;
  opts.countseq = 1;

  while ((c = getopt(argc, argv, ":csL")) != -1) {
    switch(c) {
    case 'c':
      opts.countnuc = 1;
      opts.cgiven = 1;
      if (!opts.sgiven) {
        opts.countseq = 0;
      }
      break;
    case 's':
      opts.countseq = 1;
      opts.sgiven = 1;
      if (!opts.cgiven) {
        opts.countnuc = 0;
      }
      break;
    case 'L':
      opts.countlong = 1;
      if (!opts.sgiven) {
        opts.countseq = 0;
      }
      if (!opts.cgiven) {
        opts.countnuc = 0;
      }
      break;
    case '?':
      fprintf(stderr,
              "Unrecognized option: -%c\n", optopt);
    }
  }

  return 0;
}

int main(int argc, char** argv) {
  parseopts(argc, argv);

  FILE *fp = stdin;
  char filename[256] = "";

  /* print totals if more than 1 file */
  int totals = 0;
  totals = argc - optind > 1 ? 1 : 0;

  /* no filename arguments left after opt parsing, read from stdin */
  if (optind == argc) {
    argv[optind] = "-";
    argc++;
  }

  int tots = 0; // total number of sequences (in all files)
  int totn = 0; // total number of nucleotides (in all files)
  int maxl = 0; // longest sequence (in all files)

  int max_width = 3; // default width of table column

  // this shouldnt be on the stack, should be malloc'd TODO
  int counts[3][MAX_FILES]; // holds counts
  char filenames[MAX_FILENAME_LEN][MAX_FILES]; //holds filenames for printing table
  int line_i = 0; // lines_read

  while (optind < argc) {
    strcpy(filename,argv[optind++]);
    if (strcmp(filename, "-")) {
      fp = fopen(filename, "r");
    } else {
      fp = stdin;
      strcpy(filename, "");
    }

    if (fp == NULL) {
      fprintf(stderr, "Unable to open file\n");
      return 1;
    }

    char line[MAX_LINE_LEN]; // holds line to read
    int sc = 0; // no. of sequences in this file
    int nuc = 0; // no. of nucleotides in this file
    int curseq = 0; // length of sequence being read
    int longest = 0; // length of longest sequence
    
    while (fgets(line, MAX_LINE_LEN, fp) != NULL) { //TODO switch to fgets
      if (line[0] == '>') { // FASTA header, begin new sequence
	sc++;
	longest = curseq > longest ? curseq : longest;
	curseq = 0;
      } else {
	nuc    += strlen(line);
	curseq += strlen(line);
      }
    }


    /* get max char length of numbers for formatting the final table with totals*/
    if (opts.countlong) {
      max_width = digitlen(longest) <= max_width ? max_width : digitlen(longest);
    }
    
    if (opts.countnuc) {
      max_width = digitlen(nuc) <= max_width ? max_width : digitlen(nuc);
    }

    if (opts.countseq) {
      max_width = digitlen(sc) <= max_width ? max_width : digitlen(sc);
    }

    counts[0][line_i] = longest;
    counts[1][line_i] = nuc;
    counts[2][line_i] = sc;
    strcpy(filenames[line_i++],filename);

    fclose(fp);

    tots += sc;
    totn += nuc;

    maxl = longest > maxl ? longest : maxl;
  }

  /* if we are printing totals we need those character widths too */
  if (totals) {
    if (opts.countlong) {
      max_width = digitlen(maxl) <= max_width ? max_width : digitlen(maxl);
    }
    if (opts.countnuc) {
      max_width = digitlen(totn) <= max_width ? max_width : digitlen(totn);
    }
    if (opts.countseq) {
      max_width = digitlen(tots) <= max_width ? max_width : digitlen(tots);
    }
  }

  /* print the whole lot */
  int i;
  for (i = 0; counts[0][i]; i++) {
    printf(" ");
    if (opts.countlong) {
      printf("%*d ", max_width, counts[0][i]);
    }
    if (opts.countnuc) {
      printf("%*d ", max_width, counts[1][i]);
    }
    if (opts.countseq) {
      printf("%*d ", max_width, counts[2][i]);
    }
    printf("%s\n", filenames[i]);
  }  

  if (totals) {

    printf(" ");

    if (opts.countlong) {
      printf("%*d ", max_width, maxl);
    }
    if (opts.countnuc) {
      printf("%*d ", max_width, totn);
    }
    if (opts.countseq) {
      printf("%*d ", max_width, tots);
    }
    printf("total\n");
  }

  return 0;
}

