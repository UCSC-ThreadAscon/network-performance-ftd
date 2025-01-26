#pragma once

#include "esp_sleep.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_openthread_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/logging.h"
#include "openthread/tasklet.h"
#include "openthread/coap_secure.h"
#include "openthread/thread_ftd.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>