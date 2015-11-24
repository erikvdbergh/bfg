#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "../util.h"
//#include "../subseq_func.h"
//#include "../seqc_func.h"
//#include "../sgrep_func.h"
#include "../revcomp_func.h"

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

// nifty testing functions
int filediff(char *file1, char *file2) {
  char cmd[256];
  strcpy(cmd, "diff ");
  strcat(cmd, file1);
  strcat(cmd, " ");
  strcat(cmd, file2);
  strcat(cmd, " > diffout");

  //fprintf(stderr, "running cmd: %s\n", cmd);

  system(cmd);

  struct stat fileStat;
  if (stat("diffout", &fileStat) < 0)
    exit(1);

  return (int)fileStat.st_size;
}

int fileexists(char *file) {
  struct stat fileStat;
  if (stat(file, &fileStat) < 0) {
    CU_FAIL("A testing file was not found");
    return 0;
  } else {
    return 1;
  }
}
/****************************************
*					*
*		UNIT TESTS		*
*					*
****************************************/
// util tests
void test_open_fasta(void) {
  if (
      !fileexists("testdata/test.fa")
     ) {
    return;
  }
  FILE *testfa;
  CU_ASSERT_PTR_NOT_NULL(testfa = open_fasta("testdata/test.fa", 0, 0));
}

void test_newFastaSeq(void) {
  FastaSeq *seq = newFastaSeq();
  CU_ASSERT_PTR_NOT_NULL(seq);
  deleteFastaSeq(seq);
}

void test_growFastaSeq(void) {
  FastaSeq *seq = newFastaSeq();
  CU_ASSERT_PTR_NOT_NULL(seq);
  CU_ASSERT_FALSE(growFastaSeq(seq, 0));
  deleteFastaSeq(seq);
}

void test_seq_next(void) {
  if (
      !fileexists("testdata/test.fa") ||
      !fileexists("testdata/test_multiline.fa") ||
      !fileexists("testdata/testbigseq.fa")) {
    return;
  }
  FILE *testfa = open_fasta("testdata/test.fa", 0, 0);
  FastaSeq *seq = newFastaSeq();

  CU_ASSERT_FALSE(seq_next(testfa, seq, 0));
  CU_ASSERT_STRING_EQUAL(seq->header, ">seq1");
  CU_ASSERT_STRING_EQUAL(seq->seq, "ACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTAGCACTGCTGACTA");

  clearFastaSeq(seq);

  CU_ASSERT_FALSE(seq_next(testfa, seq, 0));
  CU_ASSERT_STRING_EQUAL(seq->header, ">seq2 word word word");
  CU_ASSERT_STRING_EQUAL(seq->seq, "ACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTAGC");
  clearFastaSeq(seq);

  fclose(testfa);
  testfa = open_fasta("testdata/test_multiline.fa", 0, 0);

  CU_ASSERT_FALSE(seq_next(testfa, seq, 0));
  CU_ASSERT_STRING_EQUAL(seq->header, ">seq1");
  CU_ASSERT_STRING_EQUAL(seq->seq, "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  clearFastaSeq(seq);

  CU_ASSERT_FALSE(seq_next(testfa, seq, 0));
  CU_ASSERT_STRING_EQUAL(seq->header, ">seq2");
  CU_ASSERT_STRING_EQUAL(seq->seq, "123456789ABCDEFGHI");
  clearFastaSeq(seq);

  CU_ASSERT_TRUE(seq_next(testfa, seq, 0));
  clearFastaSeq(seq);

  fclose(testfa);
  testfa = open_fasta("testdata/testbigseq.fa", 0, 0);

  CU_ASSERT_FALSE(seq_next(testfa, seq, 0));
  CU_ASSERT_STRING_EQUAL(seq->header, ">seq3");
  CU_ASSERT_STRING_EQUAL(seq->seq, "ACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGCTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTG");
  clearFastaSeq(seq);

  CU_ASSERT_FALSE(seq_next(testfa, seq, 0));
  CU_ASSERT_STRING_EQUAL(seq->header, ">seq4");
  CU_ASSERT_STRING_EQUAL(seq->seq, "1234142143142341231423142414231432143241241424242442413213241423412314241421431223413234123412312342341421431423412314231424142314321432412414242424424132132414234123142414214312234132341234123123423414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234234142143142341231423142414231432143241241424242442413213241423412314241421431223413234123412312342341421431423412314231424142314321432412414242424424132132414234123142414214312234132341234123123423414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234234142143142341231423142414231432143241241424242442413213241423412314241421431223413234123412312342341421431423412314231424142314321432412414242424424132132414234123142414214312234132341234123123423414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234234142143142341231423142414231432143241241424242442413213241423412314241421431223413234123412312342341421431423412314231424142314321432412414242424424132132414234123142414214312234132341234123123423414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234234142143142341231423142414231432143241241424242442413213241423412314241421431223413234123412312342341421431423412314231424142314321432412414242424424132132414234123142414214312234132341234123123423414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234234142143142341231423142414231432143241241424242442413213241423412314241421431223413234123412312342341421431423412314231424142314321432412414242424424132132414234123142414214312234132341234123123423414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234234142143142341231423142414231432143241241424242442413213241423412314241421431223413234123412312342341421431423412314231424142314321432412414242424424132132414234123142414214312234132341234123123423414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234234142143142341231423142414231432143241241424242442413213241423412314241421431223413234123412312342341421431423412314231424142314321432412414242424424132132414234123142414214312234132341234123123423414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234234142143142341231423142414231432143241241424242442413213241423412314241421431223413234123412312342341421431423412314231424142314321432412414242424424132132414234123142414214312234132341234123123423414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234234142143142341231423142414231432143241241424242442413213241423412314241421431223413234123412312342341421431423412314231424142314321432412414242424424132132414234123142414214312234132341234123123423414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234");
  clearFastaSeq(seq);
}

// subseq tests
void test_substr(void) {}
void test_printseq(void) {}
void test_print_header(void) {}
void test_process_file(void) {}

// seqc tests
void test_digitlen(void) { }

// sgrep tests
void test_printmatch(void) {}
void test_compile_regexes(void) {}
void test_getfilematch(void) {}
void test_read_regex_file(void) {}

// revcomp tests
void test_reverse_str(void) {
  char teststring[] = "BIHello!";
  CU_ASSERT_STRING_EQUAL(reverse_str(teststring), "!olleHIB");
}

void test_complement(void) {
  char teststring[] = "ACTGGGTATCGACGTATGCA";
  CU_ASSERT_STRING_EQUAL(complement(teststring), "TGACCCATAGCTGCATACGT");
}

// validation suite

void test_revcomp(void) {
  if (!fileexists("bin/revcomp") ||
      !fileexists("testdata/testall.fa") ||
      !fileexists("test/results/revtest1")) {
    return;
  }
  // regular file way
  system("bin/./revcomp testdata/testall.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/revtest1"));

  // pipe through stdin 
  system("cat testdata/testall.fa | bin/./revcomp > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/revtest1"));
}

void test_subseq(void) {

}

void test_sgrep(void) {
  if (!fileexists("bin/sgrep") ||
      !fileexists("testdata/testall.fa") ||
      !fileexists("test/results/sgreptest1") ||
      !fileexists("test/results/sgreptest2")) {
    return;
  }

  // regular file way with -e
  system("bin/./sgrep -e \"seq2\" testdata/testall.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/sgreptest1"));

  // pipe through stdin
  system("cat testdata/testall.fa | bin/./sgrep -e \"seq2\" > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/sgreptest1"));

  // two regexps
  system("bin/./sgrep -e \"seq3\" -e \"seq2\" testdata/testall.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/sgreptest2"));
}

void test_seqc(void) {

}

int main(void) {
   CU_pSuite uSuite = NULL;
   CU_pSuite rcmpSuite = NULL;
   CU_pSuite seqcSuite = NULL;
   CU_pSuite sgrepSuite = NULL;
   CU_pSuite sseqSuite = NULL;
   CU_pSuite vSuite = NULL;

   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   uSuite = CU_add_suite( "util_test_suite", init_suite, clean_suite );
   if ( NULL == uSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (
        (NULL == CU_add_test(uSuite, "test_open_fasta", test_open_fasta)) ||
        (NULL == CU_add_test(uSuite, "test_newFastaSeq", test_newFastaSeq)) || 
        (NULL == CU_add_test(uSuite, "test_growFastaSeq", test_growFastaSeq)) ||
        (NULL == CU_add_test(uSuite, "test_seq_next", test_seq_next))
   ) {
     CU_cleanup_registry();
     return CU_get_error();
   }

   /* add revcomp suite to the registry */
   rcmpSuite = CU_add_suite( "revcomp_test_suite", init_suite, clean_suite );
   if ( NULL == rcmpSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (
        (NULL == CU_add_test(rcmpSuite, "test_reverse_str", test_reverse_str)) ||
        (NULL == CU_add_test(rcmpSuite, "test_complement", test_complement))
   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add subseq suite to the registry */
   seqcSuite = CU_add_suite( "seqc_test_suite", init_suite, clean_suite );
   if ( NULL == seqcSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (
        (NULL == CU_add_test(seqcSuite, "test_digitlen", test_digitlen))
   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add sgrep suite to the registry */
   sgrepSuite = CU_add_suite( "sgrep_test_suite", init_suite, clean_suite );
   if ( NULL == sgrepSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (
        (NULL == CU_add_test(sgrepSuite, "test_compile_regexes", test_compile_regexes)) ||
        (NULL == CU_add_test(sgrepSuite, "test_getfilematch", test_getfilematch)) ||
        (NULL == CU_add_test(sgrepSuite, "test_read_regex_file", test_read_regex_file))
   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add subseq suite to the registry */
   sseqSuite = CU_add_suite( "subseq_test_suite", init_suite, clean_suite );
   if ( NULL == sseqSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (
        (NULL == CU_add_test(sseqSuite, "test_substr", test_substr)) || 
        (NULL == CU_add_test(sseqSuite, "test_printseq", test_printseq)) ||
        (NULL == CU_add_test(sseqSuite, "test_print_header", test_print_header)) ||
        (NULL == CU_add_test(sseqSuite, "test_process_file", test_process_file))
   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* final validation suite */
   vSuite = CU_add_suite( "validation_suite", init_suite, clean_suite );
   if ( NULL == vSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ( (NULL == CU_add_test(vSuite, "test_revcomp", test_revcomp)) ||
        (NULL == CU_add_test(vSuite, "test_seqc", test_seqc)) ||
        (NULL == CU_add_test(vSuite, "test_sgrep", test_sgrep)) ||
        (NULL == CU_add_test(vSuite, "test_subseq", test_subseq))
   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // Run all tests using the basic interface
   CU_basic_set_mode(CU_BRM_VERBOSE);
   fprintf(stderr, "Going to run tests\n");
   CU_basic_run_tests();
   //CU_basic_run_suite(rcmpSuite);
   //CU_basic_run_suite(seqcSuite);
   //CU_basic_run_suite(sgrepSuite);
   //CU_basic_run_suite(sseqSuite);
   //CU_basic_run_suite(vSuite);
   printf("\n");
   CU_basic_show_failures(CU_get_failure_list());
   printf("\n\n");
   /*
     // Run all tests using the automated interface
     CU_automated_run_tests();
     CU_list_tests_to_file();

     // Run all tests using the console interface
     CU_console_run_tests();
   */
   /* Clean up registry and return */
   CU_cleanup_registry();
   return CU_get_error();
}
