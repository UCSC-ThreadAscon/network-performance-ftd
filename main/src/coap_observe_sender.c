#include "observe.h"
#include "time_api.h"

static esp_timer_handle_t timer;

void sendNotificationCallback(void *args)
{
  Subscription *subPtr = (Subscription *) args;

  otMessage *aRequest = (otMessage *) &(subPtr->requestBytes);
  otMessageInfo *aRequestInfo = &(subPtr->requestInfo);

  uint32_t payload = 0;
  createRandomPayload((uint8_t *) &payload);

  sendNotification(aRequest, aRequestInfo, &payload, sizeof(uint32_t));
  otLogNotePlat("Sent Notification for token %llx.", getToken(aRequest));
  return;
}

/**
 * This function is a modified version of the example source code for printing
 * the status of an OpenThread SED on light sleep:
 * https://github.com/espressif/esp-idf/blob/master/examples/openthread/ot_sleepy_device/light_sleep/main/esp_ot_sleepy_device.c#L168-L178
 */
void startSendNotifications(Subscription *subPtr)
{
  const esp_timer_create_args_t timer_args =
  {
    .name = "send_observe_notification",
    .arg  = (void*) subPtr,
    .callback = (void *) sendNotificationCallback,
    .skip_unhandled_events = true,
  };
  uint64_t intervalUs = SECONDS_TO_US(NOTIFICATION_INTERVAL_SECONDS);

  ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer, intervalUs));

  otMessage *aRequest = (otMessage *) &(subPtr->requestBytes);
  otLogNotePlat("Subscription started for token %llx.", getToken(aRequest));
  return;
}

void stopSendNotifications(Subscription *subPtr)
{
  ESP_ERROR_CHECK(esp_timer_stop(timer));
  otMessage *aRequest = (otMessage *) &(subPtr->requestBytes);
  otLogNotePlat("Cancelled subscription for token %llx.", getToken(aRequest));
  return;
}