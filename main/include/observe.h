#pragma once

#include "independent_variables.h"

#include <openthread/thread_ftd.h>

/**
 * https://datatracker.ietf.org/doc/html/rfc7641#section-2
 */
#define OBSERVE_SUBSCRIBE 0
#define OBSERVE_CANCEL 1

#define THROUGHPUT_OBSERVE_NAME "Throughput Observe"
#define PACKET_LOSS_OBSERVER_NAME "Packet Loss Observe"

#define OBSERVE_EXPERIMENTS_URI "temperature"

#define NOTIFICATION_INTERVAL_SECONDS 10

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
  otSockAddr sockAddr;
  uint64_t token;
  uint32_t sequenceNum;
}
Subscription;

void startSendNotifications(Subscription *subPtr);
void stopSendNotifications(Subscription *subPtr);

void sendTemperature(otMessage *aRequest, const otMessageInfo *aRequestInfo);

void tpObserveStartCallback(otChangedFlags changed_flags, void* ctx);