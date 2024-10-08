#include "send_server.h"

void defaultRequestHandler(void* aContext,
                           otMessage *aMessage,
                           const otMessageInfo *aMessageInfo)
{
#if CONFIG_EXPERIMENT_DEBUG
  printRequest(aMessage, aMessageInfo);
#endif
  sendCoapResponse(aMessage, aMessageInfo);
  return;
}