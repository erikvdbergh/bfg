#include <stdio.h>

FILE* open_file(char filename[]) {
  FILE *fp = fopen(filename, "r");

  if (fp == NULL) {
    fprintf(stderr, "Unable to open file\n");
    return NULL;
  }
  return fp;
}
