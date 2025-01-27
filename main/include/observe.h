#pragma once

#include "coap_workload.h"
#include "sockAddr.h"
#include "independent_variables.h"

#include <openthread/thread_ftd.h>

#define THROUGHPUT_OBSERVE_URI "throughput-observe"
#define THROUGHPUT_OBSERVE_NAME "Throughput Observe"

#define PACKET_LOSS_OBSERVE_URI "packet-loss-observe"
#define PACKET_LOSS_OBSERVER_NAME "Packet Loss Observe"

#define NOTIFICATION_INTERVAL_MS 60000 // every minute

void tpObserveStartCallback(otChangedFlags changed_flags, void* ctx);