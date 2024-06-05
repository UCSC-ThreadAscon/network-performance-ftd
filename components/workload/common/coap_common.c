/* 
 * All of the code in this file is based upon the both CoAP and CoAP secure
 * source code used as a part of the OpenThread codebase. The CoAP and
 * CoAP secure source files can be found at:
 * https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
*/
#include "workload.h"

void coapStart() {
  otError error = otCoapStart(OT_INSTANCE, COAP_SOCK_PORT);

  if (error != OT_ERROR_NONE) {
    otLogCritPlat("Failed to start COAP socket.");
  }
#if COAP_DEBUG
  else {
    otLogNotePlat("Started CoAP socket at port %d.", COAP_SERVER_PORT);
  }
#endif
  return;
}

otSockAddr createSocket(const char *recvAddrString)
{
  otSockAddr newSocket;
  otIp6Address recvAddr;

  EmptyMemory(&newSocket, sizeof(otSockAddr));
  EmptyMemory(&recvAddr, sizeof(otIp6Address));

  otIp6AddressFromString(recvAddrString, &recvAddr);

  newSocket.mAddress = recvAddr;
  newSocket.mPort = COAP_SOCK_PORT;

  return newSocket;
}