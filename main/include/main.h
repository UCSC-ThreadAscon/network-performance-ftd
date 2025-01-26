#pragma once

#include "utilities.h"
#include "coap_workload.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_openthread.h"
#include "esp_openthread_cli.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_types.h"
#include "esp_ot_config.h"
#include "esp_vfs_eventfd.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/uart_types.h"
#include "nvs_flash.h"
#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/logging.h"
#include "openthread/tasklet.h"

#include "openthread/platform/time.h"
#include "openthread/network_time.h"

/**
 * I got the idea to use this specific print statement delimiter
 * from the ESP Thread Border Router SDK source code:
 * https://github.com/espressif/esp-thread-br/blob/main/components/esp_ot_br_server/src/esp_br_web.c#L1168
 */
#define PrintDelimiter() otLogNotePlat("<===========================================================>");
#define PrintCritDelimiter() otLogCritPlat("<===========================================================>");
#define PrintWarnDelimiter() otLogWarnPlat("<===========================================================>");

#define THROUGHPUT_CONFIRMABLE (CONFIG_EXPERIMENT == 1)
#define PACKET_LOSS_CONFIRMABLE (CONFIG_EXPERIMENT == 2)
#define DELAY_SERVER (CONFIG_EXPERIMENT == 3)
#define DELAY_CLIENT (CONFIG_EXPERIMENT == 4)
#define THROUGHPUT_OBSERVE (CONFIG_EXPERIMENT == 5)
#define PACKET_LOSS_OBSERVE (CONFIG_EXPERIMENT == 6)

void startMain(void);
void resetTrials(void);
void startNextTrial(void);