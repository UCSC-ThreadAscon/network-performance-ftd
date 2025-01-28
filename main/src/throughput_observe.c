#include "observe.h"
#include "time_api.h"
#include "main.h"

#include <openthread/platform/radio.h>

static otCoapResource route;

static Subscription brSubscription;
static bool brSubscribed;

/**
 * TO-DO: Print out CoAP payloads with simulated room temperatures.
 * https://www.adt.com/resources/average-room-temperature
 * https://wmo.asu.edu/content/world-highest-temperature
 */
void tpObserveRequestHandler(void *aContext,
                             otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
  uint64_t observeValue = 0;
  otError error = coapGetOptionValue(aMessage, OT_COAP_OPTION_OBSERVE, &observeValue);
  handleError(error, "CoAP Get Observe Option Value");

  if (!brSubscribed)
  {
    if (observeValue == OBSERVE_SUBSCRIBE)
    {
      memcpy(&(brSubscription.requestBytes), aMessage, OT_RADIO_FRAME_MAX_SIZE);
      memcpy(&(brSubscription.requestInfo), aMessageInfo, sizeof(otMessageInfo));
      startSendNotifications(&brSubscription);
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
    if (observeValue == OBSERVE_CANCEL)
    {
      stopSendNotifications(&brSubscription);
      memcpy(&(brSubscription.requestBytes), aMessage, OT_RADIO_FRAME_MAX_SIZE);
      memcpy(&(brSubscription.requestInfo), aMessageInfo, sizeof(otMessageInfo));
      brSubscribed = false;
    }
    else
    {
      otLogWarnPlat("Received subscription request from token %llx when already subscribed.",
                    getToken(aMessage));
    }
  }

  sendCoapResponse(aMessage, aMessageInfo);
  return;
}

void tpObserveStartServer(void)
{
  startCoapServer(OT_DEFAULT_COAP_PORT);
  createResource(&route, THROUGHPUT_OBSERVE_NAME, THROUGHPUT_OBSERVE_URI,
                 tpObserveRequestHandler);
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