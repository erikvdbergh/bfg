#ifndef SUBSEQ_FUNC_H
#define SUBSEQ_FUNC_H

#define MAX_FILENAME_LEN 2048
#define MAX_FILES 2048
#define MAX_LINE_LEN 2048
#define MAX_SEQ_LEN 4294967296 // 4GB

struct Opts;

typedef struct OptsTag {
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
} Opts;

int substr(char *str, int start, int end); 

void printseq(char *seqid, char *seq); 

void print_header(char *seqid, Opts opts); 

int process_file(FILE *fp, Opts opts); 

#endif /* SUBSEQ_FUNC_H */
