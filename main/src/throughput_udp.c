#include "tight_loop.h"
#include "time_api.h"
#include "main.h"
#include "udp_workload.h"

#include <openthread/thread_ftd.h>
#include <openthread/logging.h>

#define STACK_SIZE 10240
#define TASK_PRIORITY 5

static otUdpSocket socket;
static otSockAddr destAddr;

TaskHandle_t tpUdpMainTask;

void tpUdpMain(void *taskParameters) {
  EmptyMemory(&socket, sizeof(otUdpSocket));
  EmptyMemory(&destAddr, sizeof(otSockAddr));

  udpCreateSocket(&socket, &destAddr);

  while (true) {
    uint8_t payload[TIGHT_LOOP_PAYLOAD_BYTES];
    EmptyMemory(&payload, sizeof(payload));
    createRandomPayload(payload);

    udpSend(&socket, payload, sizeof(payload));
    vTaskDelay(MS_TO_TICKS(UDP_MICRO_SLEEP_MS));
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

  if (!OT_INSTANCE) { return; }
  otDeviceRole role = otThreadGetDeviceRole(OT_INSTANCE);

  bool justAttached = connected(role) && (!connected(s_previous_role));
  bool justDisconnected = (!connected(role)) && connected(s_previous_role);

  if (justAttached)
  {
    otError error = otThreadBecomeLeader(OT_INSTANCE);
    if (error == OT_ERROR_NONE)
    {
      PrintDelimiter();
      otLogNotePlat("Just attached to the Thread network as the Leader.");
      otLogNotePlat("Starting to send UDP packets in a tight loop.");
      otLogNotePlat("The micro sleep is set at %d ms.", UDP_MICRO_SLEEP_MS);
      PrintDelimiter();

      xTaskCreate(tpUdpMain, "tp_udp_main", STACK_SIZE, xTaskGetCurrentTaskHandle(),
                  TASK_PRIORITY, &tpUdpMainTask);
    }
    else
    {
      PrintCritDelimiter();
      otLogCritPlat("Failed to become the Leader of the Thread Network.");
      otLogCritPlat("Going to restart.");
      PrintCritDelimiter();

      esp_restart();
    }
  }
  else if (justDisconnected)
  {
    PrintWarnDelimiter();
    otLogWarnPlat("Disconnected from the Thread network. Going to stop sending UDP packets.");
    PrintWarnDelimiter();

    vTaskDelete(tpUdpMainTask);

    otError error = otUdpClose(OT_INSTANCE, &socket);
    if (error != OT_ERROR_NONE) {
      PrintCritDelimiter();
      otLogCritPlat("Failed to close UDP socket. Restarting device.");
      PrintCritDelimiter();
    }
  }

  s_previous_role = role;
  return;
}