#include "delay.h"
#include "main.h"

static otSockAddr socket;

/**
 * If the network time isn't synchronized, wait for 1 second, then check again.
 * Repeat this process of checking and waiting until the network time is synchronized,
 * in which you can send the packets.
 */
void delayConfirmableSend(otSockAddr *socket)
{
  uint64_t networkTime = 0;
  otNetworkTimeStatus status = OT_NETWORK_TIME_UNSYNCHRONIZED;

  do {
    status = otNetworkTimeGet(OT_INSTANCE, &networkTime);
    if (status == OT_NETWORK_TIME_SYNCHRONIZED) {
      request(socket, (void *) &networkTime, DELAY_PACKET_BYTES,
              DELAY_URI, delayConfirmableResponseCallback,
              OT_COAP_TYPE_CONFIRMABLE);
    }
    else {
      PrintTimeSyncError(status);
      vTaskDelay(WAIT_TIME_TICKS);
    }
  }
  while (status != OT_NETWORK_TIME_SYNCHRONIZED);

  return;
}

/**
 * TO-DO:
 *  1. Get the Delay time from the payload, and print out the Delay.
 *  2. Get the client to stop after 1000 packets.
 *  3. Set up a sequence number to map which packet sent which delay.
 *  4. Get the client to print out the average delay after receiving 1000 packets.
 */
void delayConfirmableResponseCallback(void *aContext,
                                      otMessage *aMessage,
                                      const otMessageInfo *aMessageInfo,
                                      otError aResult)
{
  static uint32_t numPacketsReceived = 0;

  if (aResult == OT_ERROR_NONE)
  {
    uint64_t delayUs = 0;
    getPayload(aMessage, &delayUs);
    otLogNotePlat("Delay: %" PRIu64 " us", delayUs);

    numPacketsReceived += 1;
    if (numPacketsReceived < DELAY_MAX_PACKETS)
    {
      delayConfirmableSend(&socket);
    }
    else if (numPacketsReceived == 1000)
    {
      otLogNotePlat("Finished sending 1000 Delay packets.");
    }
  }
  else
  {
    otLogWarnPlat("Response error: %s", otThreadErrorToString(aResult));
  }
  return;
}

void delayConfirmableMain()
{
  InitSocket(&socket, DELAY_SERVER_IP);
  delayConfirmableSend(&socket);
  KEEP_THREAD_ALIVE();
  return;
}