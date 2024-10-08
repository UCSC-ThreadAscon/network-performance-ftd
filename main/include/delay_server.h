#pragma once

#include "workload.h"
#include "tight_loop.h"
#include "delay.h"

void delayServerMain(otChangedFlags changed_flags, void* ctx);

void defaultRequestHandler(void* aContext,
                           otMessage *aMessage,
                           const otMessageInfo *aMessageInfo);

void delayRequestHandler(void *aContext,
                         otMessage *aMessage,
                         const otMessageInfo *aMessageInfo);