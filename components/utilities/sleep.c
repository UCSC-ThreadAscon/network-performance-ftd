#include "sleep.h"

static uint64_t wakeupTimeMicro;

void initDeepSleepTimerMicro(uint64_t timeMicro)
{
  wakeupTimeMicro = timeMicro;
  ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(timeMicro));
  return;
}

bool isDeepSleepWakeup(void) {
  return esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_UNDEFINED ?
         true : false;
}

void deepSleepStart() {
#if DEEP_SLEEP_DEBUG
  double wakeupTimeMs = US_TO_MS((double) wakeupTimeMicro);
  double wakeupTimeMins = US_TO_MINUTES( (double) wakeupTimeMicro);
  otLogNotePlat("Going to deep sleep for %.3f ms, or ~%.3f minutes.",
                wakeupTimeMs, wakeupTimeMins);
#endif
  esp_deep_sleep_start();
  return;
}