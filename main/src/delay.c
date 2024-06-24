#include "delay.h"

static otSockAddr socket;

/**
 * If the network time isn't synchronized, wait for 5 seconds, then check again.
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

void delayConfirmableResponseCallback(void *aContext,
                                      otMessage *aMessage,
                                      const otMessageInfo *aMessageInfo,
                                      otError aResult)
{
  defaultResponseCallback(aContext, aMessage, aMessageInfo, aResult);
  delayConfirmableSend(&socket);   // send another request after getting response
  return;
}

void networkTimeSyncCallbback(void *aCallbackContext)
{
  otLogNotePlat("The Network Time has now been synchronized.");
  return;
}

void delayConfirmableMain()
{
  otNetworkTimeSyncSetCallback(OT_INSTANCE, networkTimeSyncCallbback, NULL);
  InitSocket(&socket);
  delayConfirmableSend(&socket);
  KEEP_THREAD_ALIVE();
  return;
}