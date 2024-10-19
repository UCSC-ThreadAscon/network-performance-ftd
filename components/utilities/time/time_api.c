/**
 * All of the code is both taken from and based upon the code
 * from the "System Time" page from the ESP-IDF documentation:
 * https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/system_time.html
*/

#include "time_api.h"
#include "assert.h"

/** https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/system_time.html#get-current-time
 */
uint64_t toUs(struct timeval time)
{
  return (uint64_t)time.tv_sec * 1000000L + (uint64_t)time.tv_usec;
}

struct timeval getTimevalNow()
{
  struct timeval tvNow;
  gettimeofday(&tvNow, NULL);
  return tvNow;
}

/**
 * What will be the timestamp "[insert number here]" seconds from now?
*/
struct timeval getFutureTimeval(uint64_t seconds)
{
  struct timeval tvFuture;
  gettimeofday(&tvFuture, NULL);
  tvFuture.tv_sec += seconds;
  return tvFuture;
}

/**
 * This function assumes that "tv2" occurs later than "tv1". The function
 * returns the time elapsed starting at "tv1" to "tv2" in microseconds.
 */
uint64_t timeDiffUs(struct timeval tv1, struct timeval tv2)
{
  uint64_t endUs = toUs(tv2);
  uint64_t startUs = toUs(tv1);
  assert(endUs >= startUs);
  return endUs - startUs;
}