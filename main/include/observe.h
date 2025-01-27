#pragma once

#include "coap_workload.h"
#include "sockAddr.h"
#include "independent_variables.h"

#include <openthread/thread_ftd.h>

#define THROUGHPUT_OBSERVE_URI "throughput-observe"
#define PACKET_LOSS_OBSERVE_URI "packet-loss-observe"

void tpObserveStartCallback(otChangedFlags changed_flags, void* ctx);