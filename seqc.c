#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048

int main(int argc, char** argv) {
  int c;
  int countnuc  = 1;
  int countseq  = 1;
  int countlong = 0;
  int sgiven = 0;
  int cgiven = 0;

  while ((c = getopt(argc, argv, ":csL")) != -1) {
    switch(c) {
    case 'c':
      countnuc = 1;
      cgiven = 1;
      if (!sgiven) {
        countseq = 0;
      }
      break;
    case 's':
      countseq = 1;
      sgiven = 1;
      if (!cgiven) {
        countnuc = 0;
      }
      break;
    case 'L':
      countlong = 1;
      if (!sgiven) {
        countseq = 0;
      }
      if (!cgiven) {
        countnuc = 0;
      }
      break;
    case '?':
      fprintf(stderr,
              "Unrecognized option: -%c\n", optopt);
    }
  }

  FILE *fp = stdin;
  char filename[256] = "";

  int totals = 0;
  totals = optind < argc ? 1 : 0;

  int tots = 0 ;
  int totn = 0 ;
  int maxl = 0 ;

  int max_width = 3;

  int counts[3][MAX_FILES];
  char filenames[MAX_FILENAME_LEN][MAX_FILES];
  int line_i = 0;

  while (optind < argc) {
    //fprintf(stderr, "opening file %s\n", argv[optind]);
    strcpy(filename,argv[optind++]);
    fp = fopen(filename, "r");

    if (fp == NULL) {
      fprintf(stderr, "Unable to open file\n");
      return 1;
    }

    char line[256];
    int sc = 0;
    int nuc = 0;
    int curseq = 0;
    int longest = 0;
    
    while (fscanf(fp, "%s", line) != EOF) {
      if (line[0] == '>') {
	sc++;
	longest = curseq > longest ? curseq : longest;
	curseq = 0;
      } else {
	nuc    += strlen(line);
	curseq += strlen(line);
      }
    }


    if (countlong) {
      max_width = digitlen(longest) <= max_width ? max_width : digitlen(longest);
    }
    
    if (countnuc) {
      max_width = digitlen(nuc) <= max_width ? max_width : digitlen(nuc);
    }

    if (countseq) {
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

  if (totals) {
    if (countlong) {
      max_width = digitlen(maxl) <= max_width ? max_width : digitlen(maxl);
    }
    if (countnuc) {
      max_width = digitlen(totn) <= max_width ? max_width : digitlen(totn);
    }
    if (countseq) {
      max_width = digitlen(tots) <= max_width ? max_width : digitlen(tots);
    }
  }

  int i;
  for (i = 0; i < line_i; i++) {
    printf(" ");
    if (countlong) {
      printf("%*d ", max_width, counts[0][i]);
    }
    if (countnuc) {
      printf("%*d ", max_width, counts[1][i]);
    }
    if (countseq) {
      printf("%*d ", max_width, counts[2][i]);
    }
    printf("%s\n", filenames[i]);
  }  

  if (totals) {

    printf(" ");

    if (countlong) {
      printf("%*d ", max_width, maxl);
    }
    if (countnuc) {
      printf("%*d ", max_width, totn);
    }
    if (countseq) {
      printf("%*d ", max_width, tots);
    }
    printf("total\n");
  }

  return 0;
}

int digitlen(int i) {
  int len = 0;
  do {
    ++len;
    i /= 10;
  } while (i);

  return len;
}
