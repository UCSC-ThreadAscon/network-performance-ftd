#include "tight_loop.h"
#include "main.h"

static otSockAddr socket;

static uint32_t packetsSent = 0;

void tpConfirmableSend(otSockAddr *socket)
{
  uint32_t payload = 0;
  createRandomPayload((uint8_t *) &payload);
  request(socket, (void *) &payload, TIGHT_LOOP_PAYLOAD_BYTES, THROUGHPUT_CONFIRMABLE_URI,
          tpConfirmableResponseCallback, OT_COAP_TYPE_CONFIRMABLE);

  packetsSent += 1;
  return;
}

void tpConfirmableResponseCallback(void *aContext,
                                   otMessage *aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   otError aResult)
{
  defaultResponseCallback(aContext, aMessage, aMessageInfo, aResult);

  if (packetsSent < MAX_PACKETS) {
    tpConfirmableSend(&socket); // send a request after getting a response.
  }
  return;
}

void tpConfirmableMain()
{
  coapStart();
  InitSocket(&socket, SERVER_IP);

  packetsSent = 0;
  tpConfirmableSend(&socket);
  return;
}

/**
 * The code for the Throughput Server start callback function comes from the ESP-IDF
 * OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void tpConfirmableStartCallback(otChangedFlags changed_flags, void* ctx)
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
    tpConfirmableMain();
  }
  s_previous_role = role;
  return;
}