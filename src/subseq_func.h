#ifndef SUBSEQ_FUNC_H
#define SUBSEQ_FUNC_H

#include "util.h"

struct SubseqOpts;

typedef struct SubseqOptsTag {
//  char files[MAX_FILES][MAX_FILENAME_LEN]; //-f --file
  int quiet; //-q --quiet --silent
  int nomsg; // -s --no-messages
  int begin; //-b --begin
  int end; // -e --end
  int header_coord; // -h --header-coord add subseq coordinates to FASTA header line

  int begingiven;
  int endgiven;
  int filegiven;
} SubseqOpts;

int subseq(FastaSeq *seq, int start, int end); 

#endif /* SUBSEQ_FUNC_H */
