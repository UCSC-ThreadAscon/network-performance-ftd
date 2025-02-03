#include "observe.h"
#include "time_api.h"
#include "main.h"

/**
 * TODO:
 * 1. The initial ACK that is sent should contain any payload. This ACK should
 *    be used to tell the client that 1000 packets are about to be sent.
 *
 * 2. After sending `MAX_PACKETS` Non-Confirmable packets, send a Confirmable packet with
 *    an empty payload. Do not send anymore Non-Confirmable temperature packets after this.
 */

static otCoapResource route;

static Subscription brSubscription;
static bool brSubscribed;

/**
 * In the Packet Loss Observe experiments, the sequence number of CoAP observe
 * notification packet plays an EXTREMELY IMPORTANT ROLE. The sequence "i"
 * of the notification indicates that the packet was the `ith` packet that has been
 * sent by the FTD.
 *
 * Note that:
 *    - The packet with sequence number 0 will be the ACK that is sent in response to the
 *      initial GET request by the border router to establish a CoAP observe subscription.
 *      This packet will only be used to indicate to the border router it is about to send
 *      Non-Confirmable packets. The ACK will NOT contain any payload.
 *
 *    - After sending Non-Confirmable packets with sequence numbers 1 to
 *      `PACKET_LOSS_OBSERVE_MAX_PACKETS`, the FTD will send a Confirmable packet
 *      with sequence number `PACKET_LOSS_OBSERVE_MAX_PACKETS + 1` (with an empty payload)
 *      to tell the Border Router that it is has finished sending all of its packets.
 *      Upon receiving this packet, the Border Router should cancel from the CoAP observe
 *      subscription, calculate the packet loss, and then start the next experiment trial.
 */
void plObserveRequestHandler(void *aContext,
                             otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
  uint64_t observeValue = 0;
  otError error = coapGetOptionValue(aMessage, OT_COAP_OPTION_OBSERVE, &observeValue);
  handleError(error, "coap get observe option value");

  if (!brSubscribed)
  {
    if (observeValue == OBSERVE_SUBSCRIBE)
    {
      brSubscribed = true;

      brSubscription.token = getToken(aMessage);
      brSubscription.tokenLength = otCoapMessageGetTokenLength(aMessage);
      brSubscription.sequenceNum = 0;

      brSubscription.sockAddr.mAddress = aMessageInfo->mPeerAddr;
      brSubscription.sockAddr.mPort = aMessageInfo->mPeerPort;

      otLogNotePlat("Subscription started with token 0x%llx.", brSubscription.token);

      sendInitialNotification(aMessage, aMessageInfo, NULL, 0, brSubscription.sequenceNum);
      brSubscription.sequenceNum += 1;
      otLogNotePlat("Sent ACK to subscriber with token 0x%llx.", brSubscription.token);

      startSendNotifications(&brSubscription);
    }
    else
    {
      otLogWarnPlat("Received cancellation from token %llx when NOT subscribed.",
                    getToken(aMessage));
      sendCoapResponse(aMessage, aMessageInfo);
    }
  }
  else // brSubscribed
  {
    if (observeValue == OBSERVE_CANCEL)
    {
      brSubscribed = false;
      sendCoapResponse(aMessage, aMessageInfo);

      otLogNotePlat("Cancelled subscription with token 0x%llx.", brSubscription.token);
      EmptyMemory(&brSubscription, sizeof(Subscription));
    }
    else
    {
      otLogWarnPlat("Received subscription request from token 0x%llx when already subscribed.",
                    getToken(aMessage));
      sendCoapResponse(aMessage, aMessageInfo);
    }
  }
  return;
}

void plObserveStartServer(void)
{
  startCoapServer(OT_DEFAULT_COAP_PORT);
  createResource(&route, PACKET_LOSS_OBSERVE_ROUTE, OBSERVE_EXPERIMENTS_URI,
                 plObserveRequestHandler);
  return;
}

/**
 * The code for the Experimental Setup server  start callback function comes from
 * the ESP-IDF OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void plObserveStartCallback(otChangedFlags changed_flags, void* ctx)
{
  OT_UNUSED_VARIABLE(ctx);
  static otDeviceRole s_previous_role = OT_DEVICE_ROLE_DISABLED;

  if (!OT_INSTANCE) { return; }
  otDeviceRole role = otThreadGetDeviceRole(OT_INSTANCE);

  if ((connected(role) == true) && (connected(s_previous_role) == false))
  {
    PrintDelimiter();
    plObserveStartServer();
    PrintDelimiter();
  }
  s_previous_role = role;
  return;
}