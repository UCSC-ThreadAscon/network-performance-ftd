#pragma once

#include "tight_loop.h"
#include "independent_variables.h"

#include <openthread/thread_ftd.h>

/**
 * https://datatracker.ietf.org/doc/html/rfc7641#section-2
 */
#define OBSERVE_SUBSCRIBE 0
#define OBSERVE_CANCEL 1

#define THROUGHPUT_OBSERVE_URI "throughput-observe"
#define THROUGHPUT_OBSERVE_NAME "Throughput Observe"

#define PACKET_LOSS_OBSERVE_URI "packet-loss-observe"
#define PACKET_LOSS_OBSERVER_NAME "Packet Loss Observe"

#define NOTIFICATION_INTERVAL_SECONDS 60

/**
 * Save a copy of the initial GET request from Border Router to subscribe to CoAP observe.
 *
 * We will assume that the GET request will be not bigger than the maximum frame size
 * of a single, unfragmented, 802.15.4 packet of 127 bytes.
 *
 * This assumption should be reasonable since the GET request should not carry any payload.
 */
typedef struct Subscription
{
  uint8_t requestBytes[OT_RADIO_FRAME_MAX_SIZE];
  otMessageInfo requestInfo;
}
Subscription;

void startSendNotifications(Subscription *args);
void stopSendNotifications();

void tpObserveStartCallback(otChangedFlags changed_flags, void* ctx);