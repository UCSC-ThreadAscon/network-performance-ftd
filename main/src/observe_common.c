#include "observe.h"
#include "esp_timer.h"

/**
 * https://github.com/espressif/esp-idf/blob/master/examples/openthread/ot_sleepy_device/light_sleep/main/esp_ot_sleepy_device.c#L168-L178
 */
void startSendNotifications()
{
  const esp_timer_create_args_t timer_args = {
    .name = "send_observe_notification",
    .arg  = NULL,
    .callback = sendNotification,
    .skip_unhandled_events = true,
  };
  return;
}