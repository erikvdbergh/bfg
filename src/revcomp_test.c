#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"
#include "revcomp.h"

int tests_run = 0;

static char *test_reverse_string() {
  char *out = reverse_str("ABCDEFGH");
  mu_assert(strcat("expected HGFEDCBA got instead: ",out), strcmp(out,"HGFEDCBA"));
  return 0;
}

static char *all_tests() {
  mu_run_test(test_reverse_string);
  return 0;
}

int main(int argc, char **argv) {
  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  } else {
    printf("All tests passed\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
