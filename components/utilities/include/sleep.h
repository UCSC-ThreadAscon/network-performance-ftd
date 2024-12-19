#pragma once

#include "time_api.h"

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "openthread/logging.h"

void initDeepSleepTimerMicro(uint64_t timeMicro);
bool isDeepSleepWakeup(void);
void deepSleepStart(void);