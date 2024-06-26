#pragma once

#define CONNECTION_WAIT_TIME_MS MS_TO_TICKS(100)
#define THREAD_WAIT_TIME MS_TO_TICKS(5000) // 5 seconds

#define WORKER_STACK_MEMORY 5120
#define WORKER_PRIORITY 5

#define COAP_SERVER_PORT CONFIG_COAP_SERVER_PORT
#define COAP_SOCK_PORT OT_DEFAULT_COAP_PORT

#define BATTERY_URI "battery"
#define EVENT_URI "event"