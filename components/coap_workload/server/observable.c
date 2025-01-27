/**
 * All of the code in this file is based upon the both CoAP and CoAP secure
 * source code used as a part of the OpenThread codebase.
 *
 * https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
 *
 * RFC 7641 states successful notifications should have a status code of "2.05 Content"
 * or "2.03 Valid":
 * https://datatracker.ietf.org/doc/html/rfc7641#section-4.2
*/
#include "coap_workload.h"

void sendNotification(otMessage *aRequest,
                      const otMessageInfo *aRequestInfo,
                      void* payload,
                      size_t payloadSize)
{
  otMessage *aResponse = NULL;

  aResponse = otCoapNewMessage(OT_INSTANCE, NULL);
  if (aResponse == NULL) {
    otLogCritPlat("Failed to initialize a new message for CoAP response.");
  }

  otError error = otCoapMessageInitResponse(aResponse, aRequest,
                                            OT_COAP_TYPE_NON_CONFIRMABLE,
                                            OT_COAP_CODE_CONTENT);
  HandleMessageError("coap message init response", aResponse, error);

  addPayload(aResponse, payload, payloadSize);

  error = otCoapSendResponse(OT_INSTANCE, aResponse, aRequestInfo);
  HandleMessageError("send response", aResponse, error);
  return;
}