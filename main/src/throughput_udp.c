#include "tight_loop.h"
#include "time_api.h"
#include "main.h"
#include "udp_workload.h"

#include <openthread/logging.h>

/**
 * TODO:
 *  1. Create the UDP socket.
 *  2. Send UDP packets infinitely to the server.
 */
static otUdpSocket socket;
static otSockAddr destAddr;

void tpUdpMain(void) {
  EmptyMemory(&socket, sizeof(otUdpSocket));
  EmptyMemory(&destAddr, sizeof(otSockAddr));

  resetTrials();
  udpCreateSocket(&socket, &destAddr);

  PrintDelimiter();
  otLogNotePlat("Starting the Throughput UDP experiment trial!");
  PrintDelimiter();

  /**
   * You need to infinite loop so that data in `socket` and `sockAddr`
   * will never be freed. The data in these two variables are needed by
   * the OpenThread worker thread.
   */
  while (true) {
    // uint8_t payload[TIGHT_LOOP_PAYLOAD_BYTES];
    // EmptyMemory(&payload, sizeof(payload));
    // createRandomPayload(payload);

    // udpSend(&socket, payload, sizeof(payload));
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    // otLogNotePlat("Sent UDP packet.");
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
    if (role != OT_DEVICE_ROLE_LEADER) {
      tpUdpMain();
    }
    else
    {
      PrintCritDelimiter();
      otLogCritPlat("FTD failed to attach to the Thread network lead by the Border Router.");
      otLogCritPlat("Going to restart the current experiment trial.");
      PrintCritDelimiter();

      esp_restart();
    }
  }
  s_previous_role = role;
  return;
}