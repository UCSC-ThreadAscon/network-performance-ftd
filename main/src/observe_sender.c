#include "observe.h"
#include "time_api.h"

static esp_timer_handle_t timer;

/**
 * Creates payload that simulates data that would be sent from a smart home therometer.
 * This function returns a random temperature (in Fahrenheit) that is within the range
 * of room temperature: between 68-74° Fahrenheit (inclusive).
 *
 * https://www.adt.com/resources/average-room-temperature
 */
void randomTemperature(Fahrenheit *temperature)
{
  uint32_t random = (esp_random() % 7) + 68;
  memcpy(temperature, &random, sizeof(Fahrenheit));
  return;
}

void printTemperature(Fahrenheit temperature, uint64_t token)
{
  otLogNotePlat("Telling %llx that current temperature is %" PRIu8 "° Fahrenheit.",
                token, temperature);
  return;
}

void sendInitialTemperature(otMessage *aRequest,
                            const otMessageInfo *aRequestInfo,
                            Subscription *subscription)
{
  Fahrenheit temperature = 0;
  randomTemperature(&temperature);
  sendInitialNotification(aRequest, aRequestInfo, &temperature,
                          sizeof(Fahrenheit), subscription->sequenceNum);

  printTemperature(temperature, subscription->token);
  subscription->sequenceNum += 1;
  return;
}

void sendTemperature(Subscription *subscription)
{
  otMessageInfo messageInfo;
  EmptyMemory(&messageInfo, sizeof(otMessageInfo));

  messageInfo.mPeerAddr = subscription->sockAddr.mAddress;
  messageInfo.mPeerPort = subscription->sockAddr.mPort;

  Fahrenheit temperature = 0;
  randomTemperature(&temperature);

  sendNotification(&messageInfo, subscription->token, subscription->tokenLength,
                   subscription->sequenceNum, &temperature, sizeof(Fahrenheit));

  printTemperature(temperature, subscription->token);
  subscription->sequenceNum += 1;
  return;
}

void sendNotificationCallback(void *subscriptionPtr)
{
  sendTemperature((Subscription *) subscriptionPtr);
  return;
}

/**
 * This function is a modified version of the example source code for printing
 * the status of an OpenThread SED on light sleep:
 * https://github.com/espressif/esp-idf/blob/master/examples/openthread/ot_sleepy_device/light_sleep/main/esp_ot_sleepy_device.c#L168-L178
 */
void startSendNotifications(Subscription *subscription)
{
  const esp_timer_create_args_t timer_args =
  {
    .name = "send_observe_notification",
    .arg  = (void*) subscription,
    .callback = (void *) sendNotificationCallback,
    .skip_unhandled_events = true,
  };
  uint64_t intervalUs = SECONDS_TO_US(NOTIFICATION_INTERVAL_SECONDS);

  ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(timer, intervalUs));
  return;
}

void stopSendNotifications(Subscription *subscription)
{
  ESP_ERROR_CHECK(esp_timer_stop(timer));
  return;
}