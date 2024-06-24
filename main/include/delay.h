#include "tight_loop.h"

#include <openthread/platform/time.h>
#include <openthread/network_time.h> 

#define DELAY_PACKET_BYTES 8
#define WAIT_TIME_MS 5000
#define WAIT_TIME_TICKS WAIT_TIME_MS / portTICK_PERIOD_MS

#define PrintTimeSyncError(status)                                          \
  if (status == OT_NETWORK_TIME_UNSYNCHRONIZED) {                           \
    otLogCritPlat("The network time is currently unsynchronized.");         \
  }                                                                         \
  else if (status == OT_NETWORK_TIME_RESYNC_NEEDED) {                       \
    otLogCritPlat("The network time needs to be resynced.");                \
  }                                                                         \
