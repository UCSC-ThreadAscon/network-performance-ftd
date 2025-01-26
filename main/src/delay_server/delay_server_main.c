#include "delay_server.h"
#include "delay.h"
#include "main.h"
#include "independent_variables.h"

static otCoapResource experimentRoute;

/**
 * This function is based upon the both CoAP and CoAP secure source code,
 * used as a part of the OpenThread codebase. The CoAP and
 * CoAP secure source files can be found at:
 * https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
*/
void delayServerSendResponse(otMessage *aRequest,
                             const otMessageInfo *aRequestInfo,
                             uint64_t delayUs,
                             uint32_t sequenceNum)
{
  otMessage *aResponse = NULL;
  otCoapCode status = OT_COAP_CODE_VALID;

  aResponse = otCoapNewMessage(OT_INSTANCE, NULL);
  if (aResponse == NULL) {
    otLogCritPlat("Failed to initialize a new message for CoAP response.");
  }

  otError error = otCoapMessageInitResponse(aResponse, aRequest, OT_COAP_TYPE_ACKNOWLEDGMENT,
                                            status);
  HandleMessageError("coap message init response", aResponse, error);

  DelayResponse response; EmptyMemory(&response, sizeof(DelayResponse));
  response.delayUs = delayUs;
  response.sequenceNum = sequenceNum;
  addPayload(aResponse, &response, sizeof(DelayResponse));

  error = otCoapSendResponse(OT_INSTANCE, aResponse, aRequestInfo);
  HandleMessageError("send response", aResponse, error);
  return;
}

void delayRequestHandler(void* aContext,
                         otMessage *aMessage,
                         const otMessageInfo *aMessageInfo)
{
  DelayRequest payload;
  getPayload(aMessage, (void *) &payload);

  uint32_t sequenceNum = payload.sequenceNum;
  uint64_t sent = payload.sent;
  uint64_t received = 0;
  uint64_t delayUs = 0;

  otNetworkTimeStatus status = otNetworkTimeGet(OT_INSTANCE, &received);

  if (status == OT_NETWORK_TIME_SYNCHRONIZED)
  {
      delayUs = received - sent;

#if CONFIG_EXPERIMENT_DEBUG
      printRequest(aMessage, aMessageInfo);
      otLogNotePlat("Packet %" PRIu32 " has Delay: %" PRIu64 " us", sequenceNum, delayUs);
#endif

      delayServerSendResponse(aMessage, aMessageInfo, delayUs, sequenceNum);
  }
  else
  {
    /**
     * Time Sync isn't working. Exit the experiment.
     */
    otLogCritPlat("Current delay test failed due to Time Sync Error.");
  }

  return;
}

/**
 * If the Network Time Sync status of the server changes at any point during the experiment,
 * print out a warning so I can investigate whether there is any issues occuring.
 *
 * This callback should set only after the server has attached to a Thread network.
 */
void serverTimeSyncChangeCallback(void *aCallbackContext)
{
  otLogWarnPlat("The Network Time state has changed.");
  return;
}

/**
 * The code for the Delay Server main function comes from the ESP-IDF
 * OpenThread SED state change callback example function:
 * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void delayServerMain(otChangedFlags changed_flags, void* ctx)
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
    SetMaxLeaderWeight();

    otNetworkTimeSyncSetCallback(esp_openthread_get_instance(), serverTimeSyncChangeCallback, NULL);

    PrintDelimiter();
    startCoapServer(OT_DEFAULT_COAP_PORT);
    createResource(&experimentRoute, "Delay Confirmable", DELAY_URI, delayRequestHandler);
    printTimeSyncPeriod();
    PrintDelimiter();
  }
  s_previous_role = role;
  return;
}