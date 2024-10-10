#include "tight_loop.h"
#include "main.h"

#define THROUGHPUT_START_SERVER_URI "throughput-start"

static otSockAddr socket;
static otCoapResource throughputStartServer;

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

  if (packetsSent < MAX_PACKETS_SENT) {
    tpConfirmableSend(&socket); // send a request after getting a response.
  }
  return;
}

/**
 * Send 1000 Confirmable CoAP requests in a tight loop as soon as the border
 * router sends a packet to the "/throughput-start" route.
 */
void throughputStartServerRequestHandler(void* aContext,
                                         otMessage *aMessage,
                                         const otMessageInfo *aMessageInfo)
{
  coapStart();
  InitSocket(&socket, SERVER_IP);

  packetsSent = 0;
  tpConfirmableSend(&socket);
  return;
}

/**
 * The code for the Delay Server main function comes from the ESP-IDF
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
    /** Set up the CoAP route that will start sending packets for the throughput
     *  experiment as soon as the border router sends a CoAP request to the route.
     */
    PrintDelimiter();
    startCoapServer(OT_DEFAULT_COAP_PORT);
    createResource(&throughputStartServer, "Throughput Experiment Start Server",
                    THROUGHPUT_START_SERVER_URI, NULL);
    PrintDelimiter();
  }
  s_previous_role = role;
  return;
}