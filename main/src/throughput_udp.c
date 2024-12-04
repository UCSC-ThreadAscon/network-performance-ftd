#include "tight_loop.h"
#include "time_api.h"
#include "main.h"

void tpUdpMain(void) {
  PrintDelimiter();
  resetTrials();
  otLogNotePlat("Starting the Throughput UDP experimental trial!");
  PrintDelimiter();
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
      otLogCritPlat("Going to restart the current experimental trial.");
      PrintCritDelimiter();

      esp_restart();
    }
  }
  s_previous_role = role;
  return;
}