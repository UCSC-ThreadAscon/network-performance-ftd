#include "tight_loop.h"
#include "time_api.h"
#include "main.h"
#include "udp_workload.h"

#include <openthread/logging.h>

#define STACK_SIZE 10240
#define TASK_PRIORITY 5

static otCoapResource server;

static otUdpSocket socket;
static otSockAddr destAddr;

static bool sendPackets;
TaskHandle_t tpUdpMainTask;

void tpUdpMain(void *taskParameters) {
  PrintDelimiter();
  otLogNotePlat("Sending UDP packets to the Border Router.");
  otLogNotePlat("The micro sleep is set at %d ms.", UDP_MICRO_SLEEP_MS);
  PrintDelimiter();

  while (true) {
    uint8_t payload[TIGHT_LOOP_PAYLOAD_BYTES];
    EmptyMemory(&payload, sizeof(payload));
    createRandomPayload(payload);

    udpSend(&socket, payload, sizeof(payload));
    vTaskDelay(MS_TO_TICKS(UDP_MICRO_SLEEP_MS));
  }
  return;
}


void toggleSendUdpPackets(void *aContext,
                          otMessage *aMessage,
                          const otMessageInfo *aMessageInfo) 
{
  getPayload(aMessage, &sendPackets);

  if (sendPackets)
  {
    xTaskCreate(tpUdpMain, "tp_udp_main", STACK_SIZE, xTaskGetCurrentTaskHandle(),
                TASK_PRIORITY, &tpUdpMainTask);
  }
  else 
  {
    PrintDelimiter();
    otLogNotePlat("Going to stop sending UDP packets to the Border Router.");
    PrintDelimiter();

    vTaskDelete(tpUdpMainTask);
  }

  return;
}

/**
 * The code for the Throughput UDP Server start callback function comes from the ESP-IDF
 * OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void tpUdpStartCallback(otChangedFlags changed_flags, void* ctx)
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
    startCoapServer(OT_DEFAULT_COAP_PORT);
    createResource(&server, "Throughput UDP Start Server", "throughput-udp",
                   toggleSendUdpPackets);

    udpCreateSocket(&socket, &destAddr);

    sendPackets = false;
  }
  s_previous_role = role;
  return;
}