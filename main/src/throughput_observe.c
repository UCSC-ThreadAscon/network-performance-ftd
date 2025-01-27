#include "observe.h"
#include "time_api.h"
#include "main.h"

#include <openthread/platform/radio.h>

static otCoapResource route;

#define RESOURCE_NAME "Throughput Observe Experiment"
#define URI "throughput-observe"

/**
 * https://datatracker.ietf.org/doc/html/rfc7641#section-2
 */
#define OBSERVE_SUBSCRIBE 0
#define OBSERVE_CANCEL 1

/**
 * Save a copy of the initial GET request from Border Router to subscribe to CoAP observe.
 *
 * We will assume that the GET request will be not bigger than the maximum frame size
 * of a single, unfragmented, 802.15.4 packet of 127 bytes.
 *
 * This assumption should be reasonable since the GET request should not carry any payload.
 */
static uint8_t requestBytes[OT_RADIO_FRAME_MAX_SIZE];
static otMessageInfo requestInfo;

static bool brSubscribed;

uint64_t getToken(otMessage *aMessage)
{
  uint64_t token = 0;
  memcpy(&token, otCoapMessageGetToken(aMessage),
         otCoapMessageGetTokenLength(aMessage));
  return token;
}

/**
 * TO-DO:
 * 1. When receiving an observe option, print out the token. (DONE)
 * 2. Send notifications to the client every minute.
 * 3. Stop sending packets when the client does a cancellation.
 */
void tpObserveRequestHandler(void *aContext,
                             otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
  const otCoapOption *observeOption = coapGetOption(aMessage, OT_COAP_OPTION_OBSERVE);

  if (observeOption != NULL)
  {
    otLogNotePlat("Observe Value: %" PRIu16 ".", observeOption->mNumber);

    if (!brSubscribed)
    {
      if (observeOption->mNumber == OBSERVE_SUBSCRIBE)
      {
        otLogNotePlat("Subscription started for token %llx.", getToken(aMessage));
        memcpy(&requestBytes, aMessage, OT_RADIO_FRAME_MAX_SIZE);
        memcpy(&requestInfo, aMessageInfo, sizeof(otMessageInfo));
        brSubscribed = true;
      }
      else
      {
        otLogWarnPlat("Received cancellation from token %llx when NOT subscribed.",
                      getToken(aMessage));
      }
    }
    else // brSubscribed
    {
      if (observeOption->mNumber == OBSERVE_CANCEL)
      {
        otLogNotePlat("Subscription has ben called for token %llx.", getToken(aMessage));
        EmptyMemory(&requestBytes, OT_RADIO_FRAME_MAX_SIZE);
        EmptyMemory(&requestInfo,  sizeof(otMessageInfo));
        brSubscribed = false;
      }
      else
      {
        otLogWarnPlat("Received subscription request from token %llx when already subscribed.",
                      getToken(aMessage));
      }
    }
  }

  sendCoapResponse(aMessage, aMessageInfo);
  return;
}

void tpObserveStartServer(void)
{
  startCoapServer(OT_DEFAULT_COAP_PORT);
  createResource(&route, RESOURCE_NAME, URI, tpObserveRequestHandler);
  return;
}

/**
 * The code for the Experimental Setup server  start callback function comes from
 * the ESP-IDF OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void tpObserveStartCallback(otChangedFlags changed_flags, void* ctx)
{
  OT_UNUSED_VARIABLE(ctx);
  static otDeviceRole s_previous_role = OT_DEVICE_ROLE_DISABLED;

  if (!OT_INSTANCE) { return; }
  otDeviceRole role = otThreadGetDeviceRole(OT_INSTANCE);

  if ((connected(role) == true) && (connected(s_previous_role) == false))
  {
    PrintDelimiter();
    tpObserveStartServer();
    PrintDelimiter();
  }
  s_previous_role = role;
  return;
}