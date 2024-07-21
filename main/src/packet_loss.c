/**
 * For packet loss, the FTD sends 1000 packet, and the Border Router counts
 * how many packets it has received from the FTD. The Border Router then calculates
 * the packet loss as:
 *
 *      Number of Packets Received
 *      --------------------------
 *      Number of Expected Packets
 *
 * where the number of expected packets is PACKET_LOSS_MAX_PACKETS_SENT.
 */
#include "tight_loop.h"

static otSockAddr socket;

void plConfirmableSend(otSockAddr *socket)
{
  static uint32_t numPacketsSent = 0;

  if (numPacketsSent < PACKET_LOSS_MAX_PACKETS_SENT)
  {
    uint32_t payload = 0;
    createRandomPayload((uint8_t *) &payload);

    request(socket, (void *) &payload, TIGHT_LOOP_PAYLOAD_BYTES, PACKET_LOSS_CONFIRMABLE_URI, 
            plConfirmableResponseCallback, OT_COAP_TYPE_CONFIRMABLE);
    numPacketsSent += 1;

#if CONFIG_EXPERIMENT_DEBUG
    otLogNotePlat("Number of Packets Sent: %" PRIu32 "", numPacketsSent);
#endif
  }
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