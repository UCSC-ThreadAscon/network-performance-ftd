#include "observe.h"
#include "time_api.h"
#include "main.h"

#include <openthread/platform/radio.h>

static otCoapResource route;

#define RESOURCE_NAME "Throughput Observe Experiment"
#define URI "throughput-observe"

/**
 * Save a copy of the initial GET request from Border Router to subscribe to CoAP observe.
 *
 * We will assume that the GET request will be not bigger than the maximum frame size
 * of a single, unfragmented, 802.15.4 packet of 127 bytes.
 *
 * This assumption should be reasonable since the GET request should not carry any payload.
 */
#define INITIAL_REQUEST_SIZE OT_RADIO_FRAME_MAX_SIZE
uint8_t requestBytes[INITIAL_REQUEST_SIZE];
otMessageInfo requestInfo;

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

  if (observeOption != NULL)  // is a CoAP observe GET request.
  {
    uint64_t token = 0;
    memcpy(&token, otCoapMessageGetToken(aMessage), otCoapMessageGetTokenLength(aMessage));
    otLogNotePlat("Received CoAP Observe request with token %llx.", token);
  
    memcpy(&requestBytes, aMessage, INITIAL_REQUEST_SIZE);
    memcpy(&requestInfo, aMessageInfo, sizeof(otMessageInfo));
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