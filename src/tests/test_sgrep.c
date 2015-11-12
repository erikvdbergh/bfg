#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "sgrep_func.h"

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/****************************************
*					*
*		UNIT TESTS		*
*					*
****************************************/
void test_printmatch(void) {}
void test_compile_regexes(void) {}
void test_getfilematch(void) {}
void test_read_regex_file(void) {}

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
   if (
         (NULL == CU_add_test(pSuite, "test_printmatch", test_printmatch)) ||
         (NULL == CU_add_test(pSuite, "test_compile_regexes", test_compile_regexes)) ||
         (NULL == CU_add_test(pSuite, "test_getfilematch", test_getfilematch)) ||
         (NULL == CU_add_test(pSuite, "test_read_regex_file", test_read_regex_file))
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
