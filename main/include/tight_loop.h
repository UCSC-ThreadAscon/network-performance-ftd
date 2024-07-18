#pragma once

#include "workload.h"
#include "socket.h"

#define THROUGHPUT_CONFIRMABLE_URI "throughput-confirmable"
#define THROUGHPUT_NONCONFIRMABLE_URI "throughput-nonconfirmable"
#define PACKET_LOSS_CONFIRMABLE_URI "packet-loss-confirmable"
#define PACKET_LOSS_NONCONFIRMABLE_URI "packet-loss-nonconfirmable"
#define DELAY_URI "delay-confirmable"

#define TIGHT_LOOP_PAYLOAD_BYTES 4

#define PACKET_LOSS_MAX_PACKETS_SENT 1000

#define NON_CONFIRMABLE_MICROSLEEP_MS 1000

void createRandomPayload(uint8_t *buffer);
void tpConfirmableMain(void);
void tpNonConfirmableMain(void);

void plConfirmableMain(void);
void plNonConfirmableMain(void);

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