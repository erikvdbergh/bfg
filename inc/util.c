#include <stdio.h>

FILE *open_file(char filename[], int nomsg, int quiet) {
  FILE *fp;

  fp = fopen(filename, "r");

  if (fp == NULL && !nomsg && !quiet) {
    fprintf(stderr, "Can't open input file %s!\n", filename);
  }

  return fp;
}
