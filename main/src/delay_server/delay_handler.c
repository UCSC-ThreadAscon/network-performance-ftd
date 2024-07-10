#include "server.h"

uint64_t getTimeSent(otMessage *aMessage)
{
  uint64_t sent = 0;
  getPayload(aMessage, (void *) &sent);
  return sent;
}

void delayRequestHandler(void* aContext,
                         otMessage *aMessage,
                         const otMessageInfo *aMessageInfo)
{
  uint64_t delayUs = 0;
  uint64_t received = 0;
  uint64_t sent = getTimeSent(aMessage);

  otNetworkTimeStatus status = otNetworkTimeGet(OT_INSTANCE, &received);
  if (status == OT_NETWORK_TIME_SYNCHRONIZED) {
      delayUs = received - sent;
      otLogNotePlat("Delay is %" PRIu64 " us.");
  }
  else {
    /**
     * Time Sync isn't working. Exit the experiment.
     */
    otLogCritPlat("Current delay test failed due to Time Sync Error.");
    return;
  }

  defaultRequestHandler(aContext, aMessage, aMessageInfo);
  return;
}

void defaultRequestHandler(void* aContext,
                           otMessage *aMessage,
                           const otMessageInfo *aMessageInfo)
{
  printRequest(aMessage, aMessageInfo);
  sendCoapResponse(aMessage, aMessageInfo);
  return;
}