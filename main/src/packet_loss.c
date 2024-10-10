/**
 * For packet loss, the FTD sends 1000 packet, and the Border Router counts
 * how many packets it has received from the FTD. The Border Router then calculates
 * the packet loss as:
 *
 *      Number of Packets Received
 *      --------------------------
 *      Number of Expected Packets
 *
 * where the number of expected packets is MAX_PACKETS.
 */
#include "tight_loop.h"

static otSockAddr socket;

void plConfirmableSend(otSockAddr *socket)
{
  static uint32_t numPacketsSent = 0;

  if (numPacketsSent < MAX_PACKETS)
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
  coapStart();
  InitSocket(&socket, SERVER_IP);
  plConfirmableSend(&socket);
  return;
}

/**
 * The code for the Packet Loss start callback comes from the ESP-IDF
 * OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void plConfirmableStartCallback(otChangedFlags changed_flags, void* ctx)
{
  OT_UNUSED_VARIABLE(ctx);
  static otDeviceRole s_previous_role = OT_DEVICE_ROLE_DISABLED;

  otInstance* instance = esp_openthread_get_instance();
  if (!instance)
  {
    return;
  }

  otDeviceRole role = otThreadGetDeviceRole(instance);
  if ((connected(role) == true) && (connected(s_previous_role) == false))
  {
    /** Start the Packet Loss experiment as soon as device attaches
     *  to the Thread network.
     */
    plConfirmableMain();
  }
  s_previous_role = role;
  return;
}