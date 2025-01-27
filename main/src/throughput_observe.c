#include "observe.h"
#include "time_api.h"
#include "main.h"

static otCoapResource route;

#define RESOURCE_NAME "Throughput Observe Experiment"
#define URI "throughput-observe"

/**
 * TO-DO:
 * Go through all the options in the CoAP response, and find out whether or not
 * the CoAP request is an observable GET request.
 */
void tpObserveRequestHandler(void *aContext,
                             otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
  printRequest(aMessage, aMessageInfo);

  const otCoapOption *observeOption = coapGetOption(aMessage, OT_COAP_OPTION_OBSERVE);
  if (observeOption != NULL) {
    otLogNotePlat("Observe Option mNumber: %" PRIu16 ".", observeOption->mNumber);
    otLogNotePlat("Observe Option mLength: %" PRIu16 ".", observeOption->mLength);
  }
  else {
    otLogWarnPlat("Observe option is NOT in response.");
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