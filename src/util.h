#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048
#define MAX_REGEX_LEN 2048
#define MAX_REGEXES 256
#define MAX_LINE_LEN 2048
#define MAX_SEQ_LEN 4294967296 // 4GB

FILE *open_fasta(char filename[], int nomsg, int quiet);

int seq_next(FILE *file, char **header, char **sequence, int quiet);

#endif /* UTIL_H */
