#include "observe.h"
#include "time_api.h"

static esp_timer_handle_t timer;

/**
 * According to the World Meteorological Organization:
 * https://wmo.asu.edu/content/world-highest-temperature
 * 
 * The highest temperature recorded was 134° Fahrenheit. As a result,
 * the temperature will be stored as an unsigned 8 bit integer.
 */
typedef uint8_t Fahrenheit;

/**
 * Creates payload that simulates data that would be sent from a smart home therometer.
 * This function returns a random temperature (in Fahrenheit) that is within the range
 * of room temperature: between 68-74° Fahrenheit (inclusive).
 *
 * https://www.adt.com/resources/average-room-temperature
 */
void getTemperature(Fahrenheit *temperature)
{
  uint32_t random = (esp_random() % 7) + 68;
  memcpy(temperature, &random, sizeof(Fahrenheit));
  return;
}

void sendTemperature(otMessage *aRequest, const otMessageInfo *aRequestInfo)
{
  Fahrenheit temperature = 0;
  getTemperature((uint8_t *) &temperature);

  sendNotification(aRequest, aRequestInfo, &temperature, sizeof(uint32_t));
  otLogNotePlat("Telling %llx that current temperature is %" PRIu8 "° Fahrenheit.",
                getToken(aRequest), temperature);
  return;
}

void sendNotificationCallback(void *args)
{
  Subscription *subPtr = (Subscription *) args;
  otMessage *aRequest = (otMessage *) &(subPtr->requestBytes);
  otMessageInfo *aRequestInfo = &(subPtr->requestInfo);
  sendTemperature(aRequest, aRequestInfo);
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