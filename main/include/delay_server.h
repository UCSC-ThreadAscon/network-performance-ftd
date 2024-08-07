#pragma once

#include "workload.h"
#include "tight_loop.h"
#include "delay.h"

otError createResource(otCoapResource *resource,
                       const char *resourceName,
                       otCoapRequestHandler requestHandler);

void resourceDestructor(otCoapResource *resource);
void sendCoapResponse(otMessage *aRequest, const otMessageInfo *aRequestInfo);
void printRequest(otMessage *aMessage, const otMessageInfo *aMessageInfo);

void delayServerMain(otChangedFlags changed_flags, void* ctx);

void defaultRequestHandler(void* aContext,
                           otMessage *aMessage,
                           const otMessageInfo *aMessageInfo);

void delayRequestHandler(void *aContext,
                         otMessage *aMessage,
                         const otMessageInfo *aMessageInfo);