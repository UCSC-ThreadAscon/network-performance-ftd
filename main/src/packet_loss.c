#include "tight_loop.h"

#define MAX_PACKETS_SENT 1000

static otSockAddr socket;

void plConfirmableSend(otSockAddr *socket)
{
  static uint32_t numPacketsSent = 0;

  if (numPacketsSent < MAX_PACKETS_SENT)
  {
    uint32_t payload = 0;
    createRandomPayload((uint8_t *) &payload);

    request(socket, (void *) &payload, TIGHT_LOOP_PAYLOAD_BYTES,
            PACKET_LOSS_CONFIRMABLE_URI, plConfirmableResponseCallback,
            OT_COAP_TYPE_CONFIRMABLE);

    numPacketsSent += 1;
  }
  return;
}

void plNonConfirmableSend(otSockAddr *socket) 
{
  uint32_t payload = 0;
  createRandomPayload((uint8_t *) &payload);

  request(socket, (void *) &payload, TIGHT_LOOP_PAYLOAD_BYTES,
          PACKET_LOSS_NONCONFIRMABLE_URI, NULL,
          OT_COAP_TYPE_NON_CONFIRMABLE);
  return;
}

void plConfirmableResponseCallback(void *aContext,
                                   otMessage *aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   otError aResult)
{
  defaultResponseCallback(aContext, aMessage, aMessageInfo, aResult);
  plConfirmableSend(&socket);   // send another request after getting a response.
}

void plConfirmableMain()
{
  InitSocket(&socket, SERVER_IP);
  plConfirmableSend(&socket);
  KEEP_THREAD_ALIVE();
  return;
}

void plNonConfirmableMain()
{
  InitSocket(&socket, SERVER_IP);
  while (true) {
    plNonConfirmableSend(&socket);
  }
  KEEP_THREAD_ALIVE();
  return;
}