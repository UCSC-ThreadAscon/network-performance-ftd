#pragma once

#include "workload.h"
#include "socket.h"

#define THROUGHPUT_CONFIRMABLE_URI "throughput-confirmable"
#define PACKET_LOSS_CONFIRMABLE_URI "packet-loss-confirmable"
#define DELAY_URI "delay-confirmable"

#define TIGHT_LOOP_PAYLOAD_BYTES 4

#define PACKET_LOSS_MAX_PACKETS_SENT 1000

void createRandomPayload(uint8_t *buffer);

void tpConfirmableMain(void);
void plConfirmableMain(void);
void delayConfirmableMain(void);

void tpConfirmableResponseCallback(void *aContext,
                                   otMessage *aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   otError aResult);
void tpConfirmableSend(otSockAddr *socket);

void plConfirmableResponseCallback(void *aContext,
                                   otMessage *aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   otError aResult);
void plConfirmableSend(otSockAddr *socket);

void delayConfirmableResponseCallback(void *aContext,
                                      otMessage *aMessage,
                                      const otMessageInfo *aMessageInfo,
                                      otError aResult);
void delayConfirmableSend(otSockAddr *socket);