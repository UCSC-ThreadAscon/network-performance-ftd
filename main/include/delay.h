#include "tight_loop.h"
#include "server.h"
#include "time_api.h"
#include "average.h"

#include <openthread/platform/time.h>
#include <openthread/network_time.h> 

#define DELAY_PACKET_BYTES 8
#define WAIT_TIME_MS 1000
#define WAIT_TIME_TICKS WAIT_TIME_MS / portTICK_PERIOD_MS
#define DELAY_MAX_PACKETS 1000

#define PrintTimeSyncError(status)                                          \
  if (status == OT_NETWORK_TIME_UNSYNCHRONIZED) {                           \
    otLogCritPlat("The network time is currently unsynchronized.");         \
  }                                                                         \
  else if (status == OT_NETWORK_TIME_RESYNC_NEEDED) {                       \
    otLogCritPlat("The network time needs to be resynced.");                \
  }                                                                         \

#define PrintDelayResults(index, delayUs)                                         \
  otLogNotePlat("The Delay for the %" PRIu32 "th packet is:", index);             \
  otLogNotePlat("%" PRIu64 " us, or", delayUs);                                   \
  otLogNotePlat("%.7f ms, or", US_TO_MS((double) delayUs));                       \
  otLogNotePlat("%.7f seconds", US_TO_SECONDS((double) delayUs));                 \

#define PrintAverageDelay(avgDelayUs)                                             \
  PrintDelimiter();                                                               \
  otLogNotePlat("The AVERAGE delay is:");                                         \
  otLogNotePlat("%" PRIu64 " us, or", avgDelayUs);                                \
  otLogNotePlat("%.7f ms, or", US_TO_MS((double) avgDelayUs));                    \
  otLogNotePlat("%.7f seconds", US_TO_SECONDS((double) avgDelayUs));              \
  PrintDelimiter();                                                               \

