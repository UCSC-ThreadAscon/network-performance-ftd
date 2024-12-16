#pragma once

#include "coap_workload.h"
#include "sockAddr.h"

#define THROUGHPUT_CONFIRMABLE_URI "throughput-confirmable"
#define PACKET_LOSS_CONFIRMABLE_URI "packet-loss-confirmable"
#define DELAY_URI "delay-confirmable"

#define THROUGHPUT_START_SERVER_URI "throughput-start"

#define TIGHT_LOOP_PAYLOAD_BYTES 4

#define MAX_PACKETS 1000

void createRandomPayload(uint8_t *buffer);

void tpConfirmableStartCallback(otChangedFlags changed_flags, void* ctx);
void plConfirmableStartCallback(otChangedFlags changed_flags, void* ctx);
void startDelayClientCallback(otChangedFlags changed_flags, void* ctx);
void tpUdpStartCallback(otChangedFlags changed_flags, void* ctx);

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