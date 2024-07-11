#include "server.h"

/**
 * This function is based upon the both CoAP and CoAP secure source code,
 * used as a part of the OpenThread codebase. The CoAP and
 * CoAP secure source files can be found at:
 * https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
*/
void delayServerSendResponse(otMessage *aRequest,
                             const otMessageInfo *aRequestInfo,
                             uint64_t delayUs)
{
  otMessage *aResponse = NULL;
  otCoapCode status = OT_COAP_CODE_VALID;

  aResponse = otCoapNewMessage(OT_INSTANCE, NULL);
  if (aResponse == NULL) {
    otLogCritPlat("Failed to initialize a new message for CoAP response.");
  }

  otError error = otCoapMessageInitResponse(aResponse, aRequest, OT_COAP_TYPE_ACKNOWLEDGMENT,
                                            status);
  HandleMessageError("coap message init response", aResponse, error);

  addPayload(aResponse, &delayUs, sizeof(uint64_t));

  error = otCoapSendResponse(OT_INSTANCE, aResponse, aRequestInfo);
  HandleMessageError("send response", aResponse, error);
  return;
}

void delayRequestHandler(void* aContext,
                         otMessage *aMessage,
                         const otMessageInfo *aMessageInfo)
{
  DelayRequest payload;
  getPayload(aMessage, (void *) &payload);

  uint64_t sent = payload.sent;
  uint32_t sequenceNum = payload.sequenceNum;
  uint64_t received = 0;
  uint64_t delayUs = 0;

  otNetworkTimeStatus status = otNetworkTimeGet(OT_INSTANCE, &received);

  if (status == OT_NETWORK_TIME_SYNCHRONIZED) {
      delayUs = received - sent;

      printRequest(aMessage, aMessageInfo);
      otLogNotePlat("Packet %" PRIu32 " has Delay: %" PRIu64 " us", sequenceNum, delayUs);

      delayServerSendResponse(aMessage, aMessageInfo, delayUs);
  }
  else {
    /**
     * Time Sync isn't working. Exit the experiment.
     */
    otLogCritPlat("Current delay test failed due to Time Sync Error.");
  }

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