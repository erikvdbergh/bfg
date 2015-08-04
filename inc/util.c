#include <stdio.h>

FILE *open_file(char filename[]) {
  FILE *fp;

  fp = fopen(filename, "r");

  if (fp == NULL) {
    fprintf(stderr, "Can't open input file %s!\n", filename);
  }

  return fp;
}
