#include "tight_loop.h"
#include "time_api.h"
#include "main.h"
#include "udp_workload.h"

#include <openthread/logging.h>

#define STACK_SIZE 10240
#define TASK_PRIORITY 5

#define MICRO_SLEEP_MS 100

/**
 * TODO:
 *  1. Create the UDP socket.
 *  2. Send UDP packets infinitely to the server.
 */
static otUdpSocket socket;
static otSockAddr destAddr;

void tpUdpMain(void *taskParameters) {
  EmptyMemory(&socket, sizeof(otUdpSocket));
  EmptyMemory(&destAddr, sizeof(otSockAddr));

  resetTrials();
  udpCreateSocket(&socket, &destAddr);

  PrintDelimiter();
  otLogNotePlat("Starting the Throughput UDP experiment trial!");
  PrintDelimiter();

  while (true) {
    uint8_t payload[TIGHT_LOOP_PAYLOAD_BYTES];
    EmptyMemory(&payload, sizeof(payload));
    createRandomPayload(payload);

    udpSend(&socket, payload, sizeof(payload));
    vTaskDelay(MS_TO_TICKS(MICRO_SLEEP_MS));
    otLogNotePlat("Sent UDP packet.");
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
    xTaskCreate(tpUdpMain, "tp_udp_main", STACK_SIZE, xTaskGetCurrentTaskHandle(),
                TASK_PRIORITY, NULL);
  }
  s_previous_role = role;
  return;
}