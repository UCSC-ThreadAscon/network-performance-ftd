/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Command Line Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/
#include "main.h"
#include "delay_server.h"
#include "tight_loop.h"
#include "observe.h"

#if CONFIG_OPENTHREAD_STATE_INDICATOR_ENABLE
#include "ot_led_strip.h"
#endif

#if CONFIG_OPENTHREAD_CLI_ESP_EXTENSION
#include "esp_ot_cli_extension.h"
#endif // CONFIG_OPENTHREAD_CLI_ESP_EXTENSION

#define TAG "ot_cli"


static esp_netif_t *init_openthread_netif(const esp_openthread_platform_config_t *config)
{
  esp_netif_config_t cfg = ESP_NETIF_DEFAULT_OPENTHREAD();
  esp_netif_t *netif = esp_netif_new(&cfg);
  assert(netif != NULL);
  ESP_ERROR_CHECK(esp_netif_attach(netif, esp_openthread_netif_glue_init(config)));

  return netif;
}

static void ot_task_worker(void *aContext)
{
  esp_openthread_platform_config_t config = {
      .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
      .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
      .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
  };

  // Initialize the OpenThread stack
  ESP_ERROR_CHECK(esp_openthread_init(&config));

#if CONFIG_OPENTHREAD_STATE_INDICATOR_ENABLE
  ESP_ERROR_CHECK(esp_openthread_state_indicator_init(esp_openthread_get_instance()));
#endif

#if CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC
  // The OpenThread log level directly matches ESP log level
  (void)otLoggingSetLevel(CONFIG_LOG_DEFAULT_LEVEL);
#endif
  // Initialize the OpenThread cli
#if CONFIG_OPENTHREAD_CLI
  esp_openthread_cli_init();
#endif

  esp_netif_t *openthread_netif;
  // Initialize the esp_netif bindings
  openthread_netif = init_openthread_netif(&config);
  esp_netif_set_default_netif(openthread_netif);

#if CONFIG_OPENTHREAD_CLI_ESP_EXTENSION
  esp_cli_custom_command_init();
#endif // CONFIG_OPENTHREAD_CLI_ESP_EXTENSION

  // Run the main loop
#if CONFIG_OPENTHREAD_CLI
  esp_openthread_cli_create_task();
#endif
#if CONFIG_OPENTHREAD_AUTO_START
  otOperationalDatasetTlvs dataset;
  otError error = otDatasetGetActiveTlvs(esp_openthread_get_instance(), &dataset);
  ESP_ERROR_CHECK(esp_openthread_auto_start((error == OT_ERROR_NONE) ? &dataset : NULL));
#endif
  setTxPower();

  /**
   * In the ESP-IDF OpenThread SED example:
   * https://github.com/UCSC-ThreadAscon/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
   *
   * The set state change callback functions is set after the call to
   * `esp_netif_set_default_netif()`. As a result, I will set all of my
   * state change callbacks in the same place.
   */
#if THROUGHPUT_CONFIRMABLE
  otSetStateChangedCallback(esp_openthread_get_instance(), tpConfirmableStartCallback, NULL);
#elif PACKET_LOSS_CONFIRMABLE
  otSetStateChangedCallback(esp_openthread_get_instance(), plConfirmableStartCallback, NULL);
#elif DELAY_SERVER
  otSetStateChangedCallback(esp_openthread_get_instance(), delayServerMain, NULL);
#elif DELAY_CLIENT
  otSetStateChangedCallback(esp_openthread_get_instance(), startDelayClientCallback, NULL);
#elif THROUGHPUT_OBSERVE
  otSetStateChangedCallback(esp_openthread_get_instance(), tpObserveStartCallback, NULL);
#elif PACKET_LOSS_OBSERVE
  otSetStateChangedCallback(esp_openthread_get_instance(), plObserveStartCallback, NULL);
#endif

  PrintDelimiter();
  printCipherSuite();
  printTxPower();
  printNetPerfTest();
  printTimeSyncStatus();
  PrintDelimiter();

  printNetworkKey();

  esp_openthread_launch_mainloop();

  // Clean up
  esp_openthread_netif_glue_deinit();
  esp_netif_destroy(openthread_netif);

  esp_vfs_eventfd_unregister();
  vTaskDelete(NULL);
}

void startMain()
{
  // Used eventfds:
  // * netif
  // * ot task queue
  // * radio driver
  esp_vfs_eventfd_config_t eventfd_config = {
      .max_fds = 3,
  };

  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));
  xTaskCreate(ot_task_worker, "ot_cli_main", 10240,
              xTaskGetCurrentTaskHandle(), 5, NULL);
  return;
}