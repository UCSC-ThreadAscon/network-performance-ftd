#pragma once

#include "coap_workload.h"

#define DELAY_SERVER_IP CONFIG_DELAY_SERVER_IP_ADDRESS
#define SERVER_IP CONFIG_SERVER_IP_ADDRESS

static inline void InitSocket(otSockAddr *socketPtr, const char* serverAddr)
{
  EmptyMemory(socketPtr, sizeof(otSockAddr));
  *socketPtr = createSocket(serverAddr);
  return;
}