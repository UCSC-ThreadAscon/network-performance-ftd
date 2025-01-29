/**
 * All of the code in this file is based upon the both CoAP and CoAP secure
 * source code used as a part of the OpenThread codebase.
 *
 * https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
*/
#include "coap_workload.h"

uint64_t getToken(otMessage *aMessage)
{
  uint64_t token = 0;
  memcpy(&token, otCoapMessageGetToken(aMessage), otCoapMessageGetTokenLength(aMessage));
  return token;
}

/**
 * Based on the implementation shown in:
 * https://github.com/openthread/openthread/blob/main/src/cli/cli_coap.cpp#L248-L276
 *
 * RFC 7641 states successful notifications should have a status code of
 * "2.05 Content" or "2.03 Valid":
 * https://datatracker.ietf.org/doc/html/rfc7641#section-4.2
 */
void sendNotification(otMessageInfo *messageInfo,
                      uint64_t token,
                      uint8_t tokenLength,
                      uint32_t sequenceNum,
                      void* payload,
                      size_t payloadSize)
{
  otMessage *notification = createCoapMessage();
  otCoapMessageInit(notification, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_CONTENT);

  otError error = otCoapMessageSetToken(notification, (const uint8_t *) &token, tokenLength);
  HandleMessageError("Setting token in CoAP observe notification", notification, error);

  error = otCoapMessageAppendObserveOption(notification, sequenceNum);
  HandleMessageError("Setting the sequence number in CoAP observe notification",
                     notification, error);
  
  error = otCoapMessageSetPayloadMarker(notification);
  HandleMessageError("Setting the payload marker in CoAP observe notification",
                     notification, error);

  error = otMessageAppend(notification, payload, payloadSize);
  HandleMessageError("Appending temperature payload to CoAP observe notification",
                     notification, error);

  error = otCoapSendRequest(OT_INSTANCE, notification, messageInfo, NULL, NULL);
  HandleMessageError("Failed to send CoAP observe notification", notification, error);
  return;
}