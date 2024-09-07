#include "average.h"

/**
 * A wrapper function for "+" (addition) that does overflow checking
 * for uint64_ts.
 */
uint64_t add(uint64_t num1, uint64_t num2)
{
  uint64_t sum = num1 + num2;
  assert(sum >= num1);
  assert(sum >= num2);
  return sum;
}

double average(uint64_t *array, int length)
{
  uint64_t sum = 0;

  for (int i = 0; i < length; i++) {
    sum = add(sum, array[i]);
  }

  double average = ((double) sum) / ((double) length);
  return average;
}
