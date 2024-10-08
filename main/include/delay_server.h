#pragma once

#include "workload.h"
#include "tight_loop.h"
#include "delay.h"

void delayServerMain(otChangedFlags changed_flags, void* ctx);

void delayRequestHandler(void *aContext,
                         otMessage *aMessage,
                         const otMessageInfo *aMessageInfo);