#include "server.h"

static uint64_t DelaysUs[DELAY_MAX_PACKETS];

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