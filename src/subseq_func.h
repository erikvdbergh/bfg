#ifndef SUBSEQ_FUNC_H
#define SUBSEQ_FUNC_H

#include "util.h"

struct SubseqOpts;

typedef struct SubseqOptsTag {
//  char files[MAX_FILES][MAX_FILENAME_LEN]; //-f --file
  char **files;
  int file_i;
  int quiet; //-q --quiet --silent
  int nomsg; // -s --no-messages
  int begin; //-b --begin
  int end; // -e --end
  int header_coord; // -h --header-coord add subseq coordinates to FASTA header line

  int begingiven;
  int endgiven;
  int filegiven;
} SubseqOpts;

int substr(char *str, int start, int end); 

void printseq(char *seqid, char *seq); 

void print_header(char *seqid, SubseqOpts opts); 

int process_file(FILE *fp, SubseqOpts opts); 

#endif /* SUBSEQ_FUNC_H */
