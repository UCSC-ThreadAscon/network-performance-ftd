#include "tight_loop.h"
#include "time_api.h"
#include "main.h"

static otSockAddr socket;

static uint32_t packetNum;
static uint32_t totalBytes;
static struct timeval startTime;
static struct timeval endTime;

static inline void resetStaticVariables()
{
  packetNum = 0;
  totalBytes = 0;
  EmptyMemory(&startTime, sizeof(struct timeval));
  EmptyMemory(&endTime, sizeof(struct timeval));
  return;
}

void tpConfirmableSend(otSockAddr *socket)
{
  uint32_t payload = 0;
  createRandomPayload((uint8_t *) &payload);
  request(socket, (void *) &payload, TIGHT_LOOP_PAYLOAD_BYTES, THROUGHPUT_CONFIRMABLE_URI,
          tpConfirmableResponseCallback, OT_COAP_TYPE_CONFIRMABLE);

  packetNum += 1;
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
    otLogCritPlat("Failed to send a packet during the experimental trial.");
    otLogCritPlat("There is a problem with this particular experimental trial.");
    otLogCritPlat("Going to restart the current experimental trial.");
    PrintCritDelimiter();

    esp_restart();
  }

  if (packetNum < MAX_PACKETS)
  {
    packetNum += 1;
    totalBytes += getPayloadLength(aMessage);

    if (packetNum == MAX_PACKETS)
    {
      /** The throughput formula is:
       *
       *      MAX_PACKETS * PAYLOAD_SIZE_BYTES
       *      -----------------------------------------   bytes/time
       *                    t_end - t_start
       * 
       */
      endTime = getTimevalNow();

      double denominatorUs = timeDiffUs(startTime, endTime);
      double denominatorMs = US_TO_MS(denominatorUs);
      double denominatorSecs = US_TO_SECONDS(denominatorUs);

      double throughputSecs = totalBytes / denominatorSecs;
      double throughputMs = totalBytes / denominatorMs;
      double throughputUs = totalBytes / denominatorUs;

      PrintDelimiter();
      otLogNotePlat("The throughput is:");
      otLogNotePlat("%.7f bytes/second, or", throughputSecs);
      otLogNotePlat("%.7f bytes/ms, or", throughputMs);
      otLogNotePlat("%.7f bytes/us.", throughputUs);
      otLogNotePlat("Duration: %.7f seconds", denominatorSecs);
      otLogNotePlat("Total Received: %" PRIu32 " bytes", totalBytes);
      PrintDelimiter();

      esp_restart();
    }
  }

  tpConfirmableSend(&socket);
  return;
}

void tpConfirmableMain()
{
  coapStart();
  InitSocket(&socket, SERVER_IP);

  resetStaticVariables();
  otLogNotePlat("Starting the throughput experiment trial!");

  startTime = getTimevalNow();
  tpConfirmableSend(&socket);
  return;
}

/**
 * The code for the Throughput Server start callback function comes from the ESP-IDF
 * OpenThread SED state change callback example function:
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