#include "tight_loop.h"
#include "time_api.h"
#include "main.h"

#define EXPECTED_TOTAL_BYTES 4000

static otSockAddr sockAddr;

static uint32_t packetsAcked;
static uint32_t totalBytes;
static struct timeval startTime;
static struct timeval endTime;

void tpConfirmableSend(otSockAddr *sockAddr)
{
  uint32_t payload = 0;
  createRandomPayload((uint8_t *) &payload);
  request(sockAddr, (void *) &payload, TIGHT_LOOP_PAYLOAD_BYTES, THROUGHPUT_CONFIRMABLE_URI,
          tpConfirmableResponseCallback, OT_COAP_TYPE_CONFIRMABLE);
  return;
}

void tpConfirmableResponseCallback(void *aContext,
                                   otMessage *aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   otError aResult)
{
  if (aResult != OT_ERROR_NONE)
  {
    PrintCritDelimiter();
    otLogCritPlat("Failed to transmit the CoAP request. Reason: %s",
                  otThreadErrorToString(aResult));
    otLogCritPlat("Going to restart the current experiment trial.");
    PrintCritDelimiter();

    esp_restart();
  }
  else
  {
    if (packetsAcked < MAX_PACKETS)
    {
      packetsAcked += 1;
      totalBytes += TIGHT_LOOP_PAYLOAD_BYTES;

      if (packetsAcked == MAX_PACKETS)
      {
        /* Check that 4000 total bytes (i.e. 4 bytes * 1000 packets)
           have been received.
        */
        assert(totalBytes == EXPECTED_TOTAL_BYTES);

        /** The throughput formula is:
         *
         *           MAX_PACKETS * PAYLOAD_SIZE_BYTES
         *      -----------------------------------------   bytes/time
         *                    t_end - t_start
         * 
         */
        endTime = getTimevalNow();

        uint64_t usElapsed = timeDiffUs(startTime, endTime);

        double denominatorUs = (double) usElapsed;
        double denominatorMs = US_TO_MS((double) denominatorUs);
        double denominatorSecs = US_TO_SECONDS((double) denominatorUs);

        double throughputSecs = (double) totalBytes / denominatorSecs;
        double throughputMs = (double) totalBytes / denominatorMs;
        double throughputUs = (double) totalBytes / denominatorUs;

        /**
         * I learned that doubles have 15 digits of precision from:
         * https://stackoverflow.com/a/2386882/6621292
         */
        PrintDelimiter();
        otLogNotePlat("The throughput is:");
        otLogNotePlat("%.15f bytes/second, or", throughputSecs);
        otLogNotePlat("%.15f bytes/ms, or", throughputMs);
        otLogNotePlat("%.15f bytes/us.", throughputUs);
        PrintDelimiter();

        PrintDelimiter();
        otLogNotePlat("Time Elapsed:");
        otLogNotePlat("%.15f seconds.", denominatorSecs);
        otLogNotePlat("%.15f ms, or", denominatorMs);
        otLogNotePlat("%" PRIu64 " us.", usElapsed);
        otLogNotePlat("Start Timestamp: %" PRIu64 "", toUs(startTime));
        otLogNotePlat("End Timestamp: %" PRIu64 "", toUs(endTime));
        PrintDelimiter();

        PrintDelimiter();
        otLogNotePlat("Total Received: %" PRIu32 " bytes", totalBytes);
        otLogNotePlat("Number of packets sent and ACKed: %" PRIu32 "", packetsAcked);
        PrintDelimiter();

        startNextTrial();
        return;
      }
    }
  }

  tpConfirmableSend(&sockAddr);
  return;
}

void tpConfirmableMain()
{
  resetTrials();
  coapStart();
  InitSockAddr(&sockAddr, SERVER_IP);

  PrintDelimiter();
  otLogNotePlat("Starting the Throughput Confirmable experiment trial!");
  PrintDelimiter();

  startTime = getTimevalNow();
  tpConfirmableSend(&sockAddr);
  return;
}

/**
 * The code for the Throughput Confirmable Server start callback function comes 
 * from the ESP-IDF OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void tpConfirmableStartCallback(otChangedFlags changed_flags, void* ctx)
{
  OT_UNUSED_VARIABLE(ctx);
  static otDeviceRole s_previous_role = OT_DEVICE_ROLE_DISABLED;

  otInstance* instance = esp_openthread_get_instance();
  if (!instance)
  {
    return;
  }

  otDeviceRole role = otThreadGetDeviceRole(instance);
  if ((connected(role) == true) && (connected(s_previous_role) == false))
  {
    tpConfirmableMain();
  }
  s_previous_role = role;
  return;
}