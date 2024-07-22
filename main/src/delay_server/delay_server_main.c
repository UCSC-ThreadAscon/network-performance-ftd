#include "delay_server.h"
#include "delay.h"
#include "main.h"

static otCoapResource experimentRoute;

void startCoapServer(uint16_t port) {
  otError error = otCoapStart(OT_INSTANCE, port);

  if (error != OT_ERROR_NONE) {
    otLogCritPlat("Failed to start COAP server.");
  } else {
    otLogNotePlat("Started CoAP server at port %d.", port);
  }
  return;
}

/**
 * The code for the Delay Server main function comes from the ESP-IDF
 * OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void delay_server_main(otChangedFlags changed_flags, void* ctx)
{
    OT_UNUSED_VARIABLE(ctx);
    otInstance* instance = esp_openthread_get_instance();
    if (!instance)
    {
        return;
    }

    otDeviceRole role = otThreadGetDeviceRole(instance);
    if ((role == OT_DEVICE_ROLE_CHILD) ||
        (role == OT_DEVICE_ROLE_ROUTER) ||
        (role == OT_DEVICE_ROLE_LEADER))
    {
      startCoapServer(OT_DEFAULT_COAP_PORT);
      createResource(&experimentRoute, "Delay Confirmable", delayRequestHandler);
    }
  return;
}