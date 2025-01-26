#pragma once

#include "coap_workload.h"
#include "sockAddr.h"

#include <openthread/thread_ftd.h>

#define THROUGHPUT_CONFIRMABLE_URI "throughput-confirmable"
#define PACKET_LOSS_CONFIRMABLE_URI "packet-loss-confirmable"
#define DELAY_URI "delay-confirmable"

#define THROUGHPUT_START_SERVER_URI "throughput-start"

#define TIGHT_LOOP_PAYLOAD_BYTES 4

#define MAX_PACKETS 1000

/**
 * Sets the Leader Weight to be the minimum on the client that the Border Router or
 * Delay server will always be the leader.
 */
#define SET_MIN_LEADER_WEIGHT()                     \
  otThreadSetLocalLeaderWeight(OT_INSTANCE, 0);     \
  otLogNotePlat("Set leader weight to %d.", 0);     \

void createRandomPayload(uint8_t *buffer);

void tpConfirmableStartCallback(otChangedFlags changed_flags, void* ctx);
void plConfirmableStartCallback(otChangedFlags changed_flags, void* ctx);
void startDelayClientCallback(otChangedFlags changed_flags, void* ctx);

void tpConfirmableResponseCallback(void *aContext,
                                   otMessage *aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   otError aResult);
void tpConfirmableSend(otSockAddr *sockAddr);

void plConfirmableResponseCallback(void *aContext,
                                   otMessage *aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   otError aResult);
void plConfirmableSend(otSockAddr *sockAddr);

void delayConfirmableResponseCallback(void *aContext,
                                      otMessage *aMessage,
                                      const otMessageInfo *aMessageInfo,
                                      otError aResult);
void delayConfirmableSend(otSockAddr *sockAddr);