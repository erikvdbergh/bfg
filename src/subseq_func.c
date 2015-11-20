#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>

#include "util.h"
#include "subseq_func.h"

int substr(char *str, int start, int end) {
  if (start > end) {
    int temp = start;
    start = end;
    end = temp;
  }

  int sl = strlen(str);
  if (end - start > sl || end > sl || start > sl) {
    //fprintf(stderr, "substr: desired length longer than string\n");
    return 1;
  }

  int sublen = end - start;
  char tempstr[sublen];
  int temp_i;
  for (temp_i = 0; temp_i < sublen; temp_i++) {
    tempstr[temp_i] = str[start + temp_i];
  }
  tempstr[temp_i] = '\0';
  strcpy(str, tempstr);

  return 0;
}

void printseq(char *seqid, char *seq) {
  if (strcmp(seqid, "") && strcmp(seq, "")) {
  }
}
void print_header(char *seqid, SubseqOpts opts) {
  if (opts.header_coord) {
    printf("%s|%i:%i", seqid, opts.begin, opts.end);
  } else {
    printf("%s", seqid);
  }
}

int process_file(FILE *fp, SubseqOpts opts) {
  char seqid[2048];

  int insub = 0;
  int nuc_pos = 0;

  int subseq_len = 0;
  int subseq_mem = 65536;

  char line[MAX_LINE_LEN];
  char *subseq = malloc(subseq_mem * sizeof(char));

  while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
    if (line[0] == '>') {
      strcpy(seqid, line);
      strcpy(subseq, "");
      nuc_pos = 1;
    } else {
      int new_nuc_pos = nuc_pos + strlen(line)-1 ;
      if (opts.begin >= nuc_pos && opts.begin < new_nuc_pos) {
        if (opts.end <= new_nuc_pos) { //substr in this one line
          substr(line, opts.begin - nuc_pos, opts.end - nuc_pos);

          print_header(seqid, opts);
          printf("%s\n", line);
        } else {
          insub = 1;
          substr(line, opts.begin - nuc_pos, new_nuc_pos - nuc_pos);

          // does not work if extracted subseq > 2048 (initial seq_mem); not sure if this will be a problem
          subseq_len += strlen(line);
          strcpy(subseq, line);

          nuc_pos = new_nuc_pos;
          continue;
        }
      }

      if (insub) {
        if (opts.end >= nuc_pos && opts.end <= new_nuc_pos) {
          substr(line, 0, opts.end - nuc_pos);

          print_header(seqid, opts);
          printf("%s%s\n", subseq, line);

          insub = 0;
        } else {
          int addlen = strlen(line);
          if (subseq_len + addlen > subseq_mem) {
            while (subseq_mem < subseq_len + addlen) {
              subseq_mem *= 2;
            }
            char *tmp = realloc(subseq, subseq_mem * sizeof(char));
            if (tmp != NULL) {
              subseq = tmp;
            } else {
              fprintf(stderr, "Unable to hold subseq in memory!");
              free(subseq);
              exit(EXIT_FAILURE);
            }
          }
          subseq_len += addlen;
          strcat(subseq, line);
        }
      }
      nuc_pos = new_nuc_pos;
    }
  }

  free(subseq);
  return 0;
}
