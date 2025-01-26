/**
 * The FTD sends 1000 packets, the Border Router ACKs each packed received, and given
 * how many packed have been packed, the FTD calculates the packet loss.
 *
 * The FTD calculates the packet loss as:
 *
 *       Number of Packets ACKED
 *      --------------------------
 *      Number of Expected Packets
 *
 * where the number of expected packets is MAX_PACKETS.
 */
#include "tight_loop.h"
#include "main.h"

static otSockAddr sockAddr;
static uint32_t numAcked;

void printPacketLoss()
{
  uint32_t numPacketsLost = MAX_PACKETS - numAcked;
  double packetLoss = ((double) numPacketsLost) / MAX_PACKETS;

  PrintDelimiter();
  otLogNotePlat("Acknowledged: %" PRIu32 " packets", numAcked);
  otLogNotePlat("Packets Lost: %" PRIu32 " packets", numPacketsLost);
  otLogNotePlat("Expected: %d packets", MAX_PACKETS);
  otLogNotePlat("Packet Loss Ratio: %.15f", packetLoss);
  PrintDelimiter();
  return;
}

void plConfirmableSend(otSockAddr *sockAddr)
{
  uint32_t payload = 0;
  createRandomPayload((uint8_t *) &payload);
  request(sockAddr, (void *) &payload, TIGHT_LOOP_PAYLOAD_BYTES, PACKET_LOSS_CONFIRMABLE_URI, 
          plConfirmableResponseCallback, OT_COAP_TYPE_CONFIRMABLE);

#if CONFIG_EXPERIMENT_DEBUG
  otLogNotePlat("Number of requests sent: %" PRIu32 "", numPacketsSent);
#endif
  return;
}

void plConfirmableResponseCallback(void *aContext,
                                   otMessage *aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   otError aResult)
{
  if (aResult == OT_ERROR_NONE)
  {
    numAcked += 1;

    if (numAcked == MAX_PACKETS)
    {
      // No packet loss.
      printPacketLoss();
      startNextTrial();
    }
    else
    {
      assert(numAcked < MAX_PACKETS);
      plConfirmableSend(&sockAddr);
    }
  }
  else
  {
    // Packet loss has occured.
    printPacketLoss();
    startNextTrial();
  }
  return;
}

void plConfirmableMain()
{
  resetTrials();
  coapStart();
  InitSockAddr(&sockAddr, SERVER_IP);
  plConfirmableSend(&sockAddr);
  return;
}

/**
 * The code for the Packet Loss start callback function comes from the ESP-IDF
 * OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void plConfirmableStartCallback(otChangedFlags changed_flags, void* ctx)
{
  OT_UNUSED_VARIABLE(ctx);
  static otDeviceRole s_previous_role = OT_DEVICE_ROLE_DISABLED;

  otInstance* instance = esp_openthread_get_instance();
  if (!instance) { return; }

  otDeviceRole role = otThreadGetDeviceRole(instance);
  if ((connected(role) == true) && (connected(s_previous_role) == false))
  {
    if (role != OT_DEVICE_ROLE_LEADER)
    {
      plConfirmableMain();
    }
    else
    {
      PrintCritDelimiter();
      otLogCritPlat("FTD failed to attach to the Thread network lead by the Border Router.");
      otLogCritPlat("Going to restart the current experiment trial.");
      PrintCritDelimiter();

      esp_restart();
    }
  }
  s_previous_role = role;
  return;
}