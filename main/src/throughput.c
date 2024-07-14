#include "tight_loop.h"

static otSockAddr socket;

void tpConfirmableSend(otSockAddr *socket)
{
  uint32_t payload = 0;
  createRandomPayload((uint8_t *) &payload);
  request(socket, (void *) &payload, TIGHT_LOOP_PAYLOAD_BYTES, THROUGHPUT_CONFIRMABLE_URI,
          tpConfirmableResponseCallback, OT_COAP_TYPE_CONFIRMABLE);
  return;
}

void tpNonConfirmableSend(otSockAddr *socket)
{
  uint32_t payload = 0;
  createRandomPayload((uint8_t *) &payload);
  request(socket, (void *) &payload, TIGHT_LOOP_PAYLOAD_BYTES, THROUGHPUT_NONCONFIRMABLE_URI,
          NULL, OT_COAP_TYPE_NON_CONFIRMABLE);
  return;
}

void tpConfirmableResponseCallback(void *aContext,
                                   otMessage *aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   otError aResult)
{
  defaultResponseCallback(aContext, aMessage, aMessageInfo, aResult);
  tpConfirmableSend(&socket); // send a request after getting a response.
  return;
}

/**
 * Create the socket, and send the first CoAP Confirmable Request.
 * All subsequent requests will be sent by the response handler.
 */
void tpConfirmableMain()
{
  InitSocket(&socket, SERVER_IP);
  tpConfirmableSend(&socket);
  KEEP_THREAD_ALIVE();
  return;
}

void tpNonConfirmableMain()
{
  InitSocket(&socket, SERVER_IP);
  while (true) {
    tpNonConfirmableSend(&socket);
  }
  KEEP_THREAD_ALIVE();
  return;
}