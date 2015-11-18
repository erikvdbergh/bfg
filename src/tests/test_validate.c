#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "../revcomp_func.h"

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/****************************************
*					*
*		UNIT TESTS		*
*					*
****************************************/

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

void test_revcomp(void) {
  if (!fileexists("bin/revcomp") ||
      !fileexists("testdata/test.fa") ||
      !fileexists("test/results/revtest1")) {
    return;
  }
  // regular file way
  system("bin/./revcomp testdata/test.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/revtest1"));

  // pipe through stdin 
  system("cat testdata/test.fa | bin/./revcomp > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/revtest1"));
}

void test_subseq(void) {

}

void test_sgrep(void) {
  if (!fileexists("bin/sgrep") ||
      !fileexists("testdata/test.fa") ||
      !fileexists("test/results/sgreptest1") ||
      !fileexists("test/results/sgreptest2")) {
    return;
  }

  // regular file way with -e
  system("bin/./sgrep -e \"seq2\" testdata/test.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/sgreptest1"));

  // pipe through stdin
  system("cat testdata/test.fa | bin/./sgrep -e \"seq2\" > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/sgreptest1"));

  // two regexps
  system("bin/./sgrep -e \"seq3\" -e \"seq2\" testdata/test.fa > out");
  CU_ASSERT_FALSE(filediff("out", "test/results/sgreptest2"));
}

void test_seqc(void) {

}

int main(void) {
  CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite( "revcomp_test_suite", init_suite, clean_suite );
   if ( NULL == pSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ( (NULL == CU_add_test(pSuite, "test_initseq", test_revcomp)) ||
        (NULL == CU_add_test(pSuite, "test_seqc", test_seqc)) ||
        (NULL == CU_add_test(pSuite, "test_sgrep", test_sgrep)) ||
        (NULL == CU_add_test(pSuite, "test_subseq", test_subseq))
   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // Run all tests using the basic interface
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_suite(pSuite);
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
