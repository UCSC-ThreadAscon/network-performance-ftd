/**
 * All of the code in this file is based upon the CoAP source code used
 * as a part of the OpenThread codebase.
 *
 * https://github.com/openthread/openthread/blob/main/src/cli/cli_coap_secure.cpp
 * https://github.com/openthread/openthread/blob/main/include/openthread/coap_secure.h
 * https://github.com/openthread/openthread/blob/main/include/openthread/coap.h
 * https://github.com/openthread/openthread/blob/main/src/cli/README_COAPS.md
*/
#include "workload.h"

#define PERIODIC_URI "periodic"

void handleResponse(void *aContext,
                    otMessage *aMessage,
                    const otMessageInfo *aMessageInfo,
                    otError aResult)
{
  if (aResult != OT_ERROR_NONE) {
    otLogWarnPlat("Response error: %s", otThreadErrorToString(aResult));
  } else {
    uint16_t payloadLen = getPayloadLength(aMessage);
    char payload[payloadLen];
    EmptyMemory(payload, payloadLen);
    getPayload(aMessage, payload);

    char senderAddr[OT_IP6_ADDRESS_STRING_SIZE];
    otIp6AddressToString(&(aMessageInfo->mPeerAddr), senderAddr,
                         OT_IP6_ADDRESS_STRING_SIZE);

    otLogNotePlat("Response from %d of size %" PRIu16 " bytes: %s.",
                  senderAddr, payload);
  }
  return;
}

void sendPeriodicRequest() {
  otMessage *aRequest = otCoapNewMessage(OT_INSTANCE, NULL);
  if (aRequest == NULL) {
    otLogCritPlat("Failed to create CoAP request.");
    return;
  }

  otCoapMessageInit(aRequest, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_POST);
  otCoapMessageGenerateToken(aRequest, OT_COAP_DEFAULT_TOKEN_LENGTH);

  otError error = otCoapMessageAppendUriPathOptions(aRequest, PERIODIC_URI);
  HandleMessageError("append uri options", aRequest, error);

  error = otCoapSecureSendRequest(OT_INSTANCE, aRequest, handleResponse, NULL);
  HandleMessageError("send request", aRequest, error);
  return;
}

void handleConnected(bool isConnected, void* context) {
  if (isConnected) {
    otLogNotePlat("DTLS conection established.");
  }
  else {
    otLogNotePlat("DTLS connection has been disconnected.");
  }
  return;
}

void clientConnect(const otSockAddr *socket) {
  char addressString[OT_IP6_ADDRESS_STRING_SIZE];
  EmptyMemory(addressString, OT_IP6_ADDRESS_STRING_SIZE);

  otIp6AddressToString(&(socket->mAddress), addressString,
                       OT_IP6_ADDRESS_STRING_SIZE);
  otLogNotePlat("Attempting DTLS connection with %s.", addressString);

  otCoapSecureConnect(OT_INSTANCE, socket, handleConnected, NULL);
  return;
}

void clientDisconect() {
  otCoapSecureDisconnect(OT_INSTANCE);
  return;
}