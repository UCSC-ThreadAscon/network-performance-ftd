#pragma once

#include "tight_loop.h"
#include "delay_server.h"
#include "time_api.h"
#include "average.h"

#include <openthread/platform/time.h>
#include <openthread/network_time.h> 

#define DELAY_PACKET_BYTES 8
#define WAIT_TIME_MS 100
#define WAIT_TIME_TICKS WAIT_TIME_MS / portTICK_PERIOD_MS

typedef struct DelayRequest
{
  uint32_t sequenceNum;
  uint64_t sent;
}
DelayRequest;

typedef struct DelayResponse
{
  uint32_t sequenceNum;
  uint64_t delayUs;
}
DelayResponse;

#define PrintTimeSyncError(status)                                          \
  if (status == OT_NETWORK_TIME_UNSYNCHRONIZED) {                           \
    otLogwarnPlat("The network time is currently unsynchronized.");         \
  }                                                                         \
  else if (status == OT_NETWORK_TIME_RESYNC_NEEDED) {                       \
    otLogwarnPlat("The network time needs to be resynced.");                \
  }                                                                         \

/**
 * I found that doubles have 15 digits of precision from:
 * https://stackoverflow.com/a/2386882/6621292
 */
#define PrintAverageDelay(avgDelayUs)                                             \
  PrintDelimiter();                                                               \
  otLogNotePlat("The AVERAGE delay is: %.15f us, or", avgDelayUs);                \
  otLogNotePlat("%.15f ms, or", US_TO_MS((double) avgDelayUs));                   \
  otLogNotePlat("%.15f seconds", US_TO_SECONDS((double) avgDelayUs));             \
  PrintDelimiter();                                                               \

