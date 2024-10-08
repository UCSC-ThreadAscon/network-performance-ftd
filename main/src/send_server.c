#include "send_server.h"

#define SEND_SERVER_URI "send"

static otCoapResource sendRoute;

/**
 * If "true", the Throughput and Packet Loss clients will continue to send
 * packets in a tight loop until the value is set to "false".
 *
 * The border router can toggle this boolean value by sending an empty NON-Confirmable
 * packet.
 */
static bool sendPackets = false;

void getSendServerBool() {
  return sendPackets;
}

void sendServerRequestHandler(void* aContext,
                              otMessage *aMessage,
                              const otMessageInfo *aMessageInfo)
{
  sendPackets = !sendPackets;
  return;
}

/**
 * The code for this  function comes from the ESP-IDF
 * OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void startSendServer(otChangedFlags changed_flags, void* ctx)
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
    PrintDelimiter();
    startCoapServer(OT_DEFAULT_COAP_PORT);
    createResource(&sendRoute, "'Send server'", SEND_SERVER_URI, sendServerRequestHandler);
    printTimeSyncPeriod();
    PrintDelimiter();
  }
  s_previous_role = role;
  return;
}