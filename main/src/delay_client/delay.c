#include "delay.h"
#include "main.h"

static otSockAddr socket;

static uint64_t DelaysUs[DELAY_MAX_PACKETS];
static uint32_t indexDelayUs;

/**
 * If the network time isn't synchronized, wait for 1 second, then check again.
 * Repeat this process of checking and waiting until the network time is synchronized,
 * in which you can send the packets.
 */
void delayConfirmableSend(otSockAddr *socket)
{
  static uint32_t sequenceNum = 0;
  uint64_t networkTime = 0;
  otNetworkTimeStatus status = OT_NETWORK_TIME_UNSYNCHRONIZED;

  do
  {
    status = otNetworkTimeGet(OT_INSTANCE, &networkTime);

    if (status == OT_NETWORK_TIME_SYNCHRONIZED)
    {
      DelayRequest payload; EmptyMemory(&payload, sizeof(DelayRequest));

      payload.sequenceNum = sequenceNum;
      payload.sent = networkTime;

      request(socket, &payload, sizeof(DelayRequest), DELAY_URI,
              delayConfirmableResponseCallback, OT_COAP_TYPE_CONFIRMABLE);
      sequenceNum += 1;
    }
    else
    {
#if CONFIG_EXPERIMENT_DEBUG
      PrintTimeSyncError(status);
#endif
    /**
     * TODO:
     *  You DO NOT need this. Use the network time callback handler, of
     *  type `otNetworkTimeSyncCallbackFn()` to only start sending packets
     *  as soon as the network time has been synchronized.
     *
     *  Then, whenever you get even ONE CALL in which `otNetworkTimeGet()`
     *  that returns an error indicating a time sync problem, you END the
     *  experiment.
     */
      vTaskDelay(WAIT_TIME_TICKS);
    }
  }
  while (status != OT_NETWORK_TIME_SYNCHRONIZED);

  return;
}

void delayConfirmableResponseCallback(void *aContext,
                                      otMessage *aMessage,
                                      const otMessageInfo *aMessageInfo,
                                      otError aResult)
{
  if (aResult == OT_ERROR_NONE)
  {
    DelayResponse payload; EmptyMemory(&payload, sizeof(DelayResponse));
    getPayload(aMessage, (void *) &payload);

    if (payload.sequenceNum == 0)
    {
      /**
       * Skip, as the delay with sequence number 0 seems to often be larger
       *  than the actual delay.
       */
      otLogNotePlat("Packet %" PRIu32 " has Delay: %" PRIu64 " us (going to skip)",
                    payload.sequenceNum, payload.delayUs);
      delayConfirmableSend(&socket);
    }
    else
    {
      /**
       * Print out the individual delays too.
       */
      DelaysUs[indexDelayUs] = payload.delayUs;
      otLogNotePlat("Packet %" PRIu32 " has Delay: %" PRIu64 " us",
                    payload.sequenceNum, DelaysUs[indexDelayUs]);

      indexDelayUs += 1;
      if (indexDelayUs < DELAY_MAX_PACKETS)
      {
        delayConfirmableSend(&socket);
      }
      else if (indexDelayUs == DELAY_MAX_PACKETS)
      {
        uint64_t averageDelayUs = average(DelaysUs, DELAY_MAX_PACKETS);
        PrintAverageDelay(averageDelayUs);

        /** The current experimental trial is over. If need be, start the
         *  next trial.
         */
        startNextTrial();
      }
    }
  }
  else
  {
    otLogWarnPlat("Response error: %s", otThreadErrorToString(aResult));
  }
  return;
}

/**
 * The code for the Delay Confirmable main function comes from the ESP-IDF
 * OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73 
 */
void delayConfirmableMain()
{
  otLogNotePlat("Hello");
  InitSocket(&socket, DELAY_SERVER_IP);
  return;
}