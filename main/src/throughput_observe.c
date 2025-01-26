#include "observe.h"
#include "time_api.h"
#include "main.h"

// static otCoapResource route;

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
    printNetworkKey();

    // Set leader weight to max to guarantee that device will be leader.
    otThreadSetLocalLeaderWeight(OT_INSTANCE, UINT8_MAX);
    otLogNotePlat("Set leader weight to %d to guarantee Leader status.", UINT8_MAX);

    otError error = otThreadBecomeLeader(OT_INSTANCE);
    if (error == OT_ERROR_NONE)
    {
      otLogNotePlat("Successfully attached to the Thread Network as the leader.");
    }
    else
    {
      PrintCritDelimiter();
      otLogCritPlat("Failed to become the Leader of the Thread Network.");
      otLogCritPlat("Reason: %s", otThreadErrorToString(error));
      otLogCritPlat("Going to restart.");
      PrintCritDelimiter();

      esp_restart();
    }

    otLogNotePlat("Going to set up the server for the Throughput Observe experiment.");
  }
  return;
}