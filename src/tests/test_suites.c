#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "../util.h"
#include "../subseq_func.h"
#include "../seqc_func.h"
#include "../sgrep_func.h"
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
  CU_ASSERT_STRING_EQUAL(seq->seq, "ACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTG");
  clearFastaSeq(seq);

  CU_ASSERT_FALSE(seq_next(testfa, seq, 0));
  CU_ASSERT_STRING_EQUAL(seq->header, ">seq4");
  CU_ASSERT_STRING_EQUAL(seq->seq, "123414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234123414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234123414214314234123142314241423143214324124142424244241321324142341231424142143122341323412341231234");
  clearFastaSeq(seq);

  fclose(testfa);
  testfa = open_fasta("testdata/testbiggerseq.fa", 0, 0);

  CU_ASSERT_FALSE(seq_next(testfa, seq, 0));
  CU_ASSERT_STRING_EQUAL(seq->header, ">seq3");
  CU_ASSERT_STRING_EQUAL(seq->seq, "ACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTGACTGAGCAGTAGCTGACTAGCTAGCGAGCTAGTCAGTCGACGAGCGCGCGGCGATCATCGAGCTGACTAGCGAGCAGTACCTGATCTGACTGACTACTG");
  clearFastaSeq(seq);
  fclose(testfa);
}

// subseq tests
void test_substr(void) {
  FastaSeq *seq = newFastaSeq();
  strcpy(seq->seq, "This is a testseq");
  subseq(seq, 5,  10);
  CU_ASSERT_STRING_EQUAL(seq->seq, " is a ");
}

// seqc tests
void test_digitlen(void) { 
  CU_ASSERT_EQUAL(digitlen(100),3);
  CU_ASSERT_EQUAL(digitlen(1000),4);
  CU_ASSERT_EQUAL(digitlen(1),1);
}

// sgrep tests
void test_compile_regexes(void) {}
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
  if (!fileexists("bin/subseq") ||
      !fileexists("testdata/testall.fa") ||
      !fileexists("test/results/subseqtest1") ||
      !fileexists("test/results/subseqtest2")) {
    return;
  }
  system("bin/./subseq -b 5 -e 10 testdata/testall.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/subseqtest1"));

  system("cat testdata/testall.fa | bin/./subseq -b 5 -e 10 > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/subseqtest1"));

  // test with header add
  system("bin/./subseq -b 5 -e 10 -h testdata/testall.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/subseqtest2"));

  // Arabidopsis TAIR test CYP79F2 gene (AT1G16400)
  // requires specific local setup, should probs not be in public code
  system("bin/./subseq ~/Dropbox/Projects_BAK/Sequences/A_thaliana/TAIR10_chr1.fas -h -b 5605159 -e 5607473 > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/CYP79F2.fa"));

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
  if (!fileexists("bin/seqc") ||
      !fileexists("testdata/testall.fa") ||
      !fileexists("test/results/seqctest1") ||
      !fileexists("test/results/seqctest2")) {
    return;
  }

  // file as argument
  system("bin/./seqc testdata/testall.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/seqctest1"));

  // file thru stdin
  system("cat testdata/testall.fa | bin/./seqc > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/seqctest2"));

  // test multiple files
  system("bin/./seqc testdata/*.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/seqctest3"));

  // test longest option
  system("bin/./seqc -L testdata/testbiggerseq.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/seqctest4"));

  system("bin/./seqc -L testdata/*.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/seqctest5"));

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
        (NULL == CU_add_test(sseqSuite, "test_substr", test_substr))
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
