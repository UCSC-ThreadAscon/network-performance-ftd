#include "delay.h"
#include "main.h"
#include "independent_variables.h"

static otSockAddr sockAddr;

static uint64_t DelaysUs[MAX_PACKETS];
static uint32_t indexDelayUs;

/**
 * If the network time isn't synchronized, wait for 100 ms, then check again.
 * Repeat this process of checking and waiting until the network time is synchronized,
 * in which you can send the packets.
 */
void delayConfirmableSend(otSockAddr *sockAddr)
{
  static uint32_t sequenceNum = 0;
  uint64_t networkTime = 0;
  otNetworkTimeStatus status = OT_NETWORK_TIME_UNSYNCHRONIZED;

  do
  {
    status = otNetworkTimeGet(OT_INSTANCE, &networkTime);

    if (status == OT_NETWORK_TIME_SYNCHRONIZED)
    {
      DelayRequest payload; EmptyMemory(&payload, sizeof(DelayRequest));

      payload.sequenceNum = sequenceNum;
      payload.sent = networkTime;

      request(sockAddr, &payload, sizeof(DelayRequest), DELAY_URI,
              delayConfirmableResponseCallback, OT_COAP_TYPE_CONFIRMABLE);
      sequenceNum += 1;
    }
    else
    {
#if CONFIG_EXPERIMENT_DEBUG
      PrintTimeSyncError(status);
#endif
      vTaskDelay(WAIT_TIME_TICKS);
    }
  }
  while (status != OT_NETWORK_TIME_SYNCHRONIZED);

  return;
}

void delayConfirmableResponseCallback(void *aContext,
                                      otMessage *aMessage,
                                      const otMessageInfo *aMessageInfo,
                                      otError aResult)
{
  if (aResult == OT_ERROR_NONE)
  {
    DelayResponse payload; EmptyMemory(&payload, sizeof(DelayResponse));
    getPayload(aMessage, (void *) &payload);

    if (payload.sequenceNum == 0)
    {
      /**
       * Skip, as the delay with sequence number 0 seems to often be larger
       * than the actual delay.
       */
      otLogNotePlat("Packet %" PRIu32 " has Delay: %" PRIu64 " us (going to skip)",
                    payload.sequenceNum, payload.delayUs);
      delayConfirmableSend(&sockAddr);
    }
    else
    {
      /**
       * Print out the individual delays too.
       */
      DelaysUs[indexDelayUs] = payload.delayUs;
      otLogNotePlat("Packet %" PRIu32 " has Delay: %" PRIu64 " us",
                    payload.sequenceNum, DelaysUs[indexDelayUs]);

      indexDelayUs += 1;
      if (indexDelayUs < MAX_PACKETS)
      {
        delayConfirmableSend(&sockAddr);
      }
      else if (indexDelayUs == MAX_PACKETS)
      {
        double averageDelayUs = average(DelaysUs, MAX_PACKETS);
        PrintAverageDelay(averageDelayUs);

        /** The current experiment trial is over. If need be, start the
         *  next trial.
         */
        startNextTrial();
      }
    }
  }
  else
  {
    otLogWarnPlat("Response error: %s", otThreadErrorToString(aResult));

    PrintCritDelimiter();
    otLogCritPlat("Failed to send a Delay packet during the experiment.");
    otLogCritPlat("Going to restart the current experiment trial.");
    PrintCritDelimiter();

    esp_restart();
  }
  return;
}

void delayConfirmableMain(void *aCallbackContext)
{
  static bool trialStarted = false;

  if (!trialStarted)
  {
    trialStarted = true;

    PrintDelimiter();
    resetTrials();
    coapStart();
    printTimeSyncPeriod();
    PrintDelimiter();

    InitSockAddr(&sockAddr, DELAY_SERVER_IP);
    delayConfirmableSend(&sockAddr);
  }
  else
  {
    PrintCritDelimiter();
    otLogCritPlat("The Network Time Sync status changed while the current experiment trial is running!");
    otLogCritPlat("There is a problem with this particular experiment trial.");
    otLogCritPlat("Going to restart the current experiment trial.");
    PrintCritDelimiter();

    esp_restart();
  }
  return;
}

/**
 * The code for starting the CoAP server when the device has attached to a Thread network
 * comes from the ESP-IDF OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void startDelayClientCallback(otChangedFlags changed_flags, void* ctx)
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
    SET_MIN_LEADER_WEIGHT();

    if (role != OT_DEVICE_ROLE_LEADER)
    {
      otNetworkTimeSyncSetCallback(esp_openthread_get_instance(), delayConfirmableMain, NULL);
    }
    else
    {
      PrintCritDelimiter();
      otLogCritPlat("Delay Client failed to attach to the Thread network lead by the Delay Server.");
      otLogCritPlat("Going to restart the current experiment trial.");
      PrintCritDelimiter();

      esp_restart();
    }
  }
  s_previous_role = role;
  return;
}