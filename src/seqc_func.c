#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

/*
 * Quick method to count the amount of digits in a number
 * @param i number to count digits
 * @return amount of digits in number
 */
int digitlen(int i) {
  int len = 0;
  do {
    ++len;
    i /= 10;
  } while (i);

  return len;
}
