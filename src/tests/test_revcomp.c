#include <stdio.h>
#include <string.h>
#include <malloc.h>
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
Opts opts;

void test_initseq(void) {
  char *seq = NULL;
  int seqsize = 20;
  CU_ASSERT_FALSE(initseq(&seq, seqsize, opts));
  CU_ASSERT_PTR_NOT_NULL(seq);
}

void test_growseq(void) {
  int seqsize = 80;
  char *seq = calloc(seqsize, sizeof(char));
  CU_ASSERT_FALSE(initseq(&seq, seqsize, opts));
  CU_ASSERT_PTR_NOT_NULL(seq);
}

void test_reverse_str(void) {
  char teststring[] = "Hello!";
  CU_ASSERT_STRING_EQUAL(reverse_str(teststring), "!olleH");
}

void test_complement(void) {
  char teststring[] = "ACTGGGTATCGACGTATGCA";
  CU_ASSERT_STRING_EQUAL(complement(teststring), "TGACCCATAGCTGCATACGT");
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
   if ( (NULL == CU_add_test(pSuite, "test_initseq", test_initseq)) ||
        (NULL == CU_add_test(pSuite, "test_growseq", test_growseq)) ||
        (NULL == CU_add_test(pSuite, "test_reverse_str", test_reverse_str)) ||
        (NULL == CU_add_test(pSuite, "test_complement", test_complement))
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
