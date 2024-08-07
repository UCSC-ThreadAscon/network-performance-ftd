#include "workload.h"

#include <openthread/thread.h>
#include <openthread/platform/radio.h>
#include <assert.h>

void handleError(otError error, char* desc)
{
  if (error != OT_ERROR_NONE) {
    otLogCritPlat("%s error: %s", desc, otThreadErrorToString(error));
  }
  return;
}

uint16_t getPayloadLength(const otMessage *aMessage) {
  return otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
}

/**
 * Based upon the OpenThread CLI UDP Handler code.
 * https://github.com/openthread/openthread/blob/main/src/cli/cli_udp.cpp#L461
 *
 * According to the handler code, `otMessageGetOffset()` returns the end
 * of the Message header, while `otMessageGetLength()` returns the size
 * of the Message header, payload included.
*/
void getPayload(const otMessage *aMessage, void* buffer)
{
  uint16_t offset = otMessageGetOffset(aMessage);
  uint16_t length = getPayloadLength(aMessage);

  uint16_t bytesRead = otMessageRead(aMessage, offset, buffer, length);
  assert(bytesRead == length);
  return;
}

/**
 * Printing out Mesh Local EID, as this is the recommended IPv6 address
 * to be used at the application layer.
 * https://openthread.io/guides/thread-primer/ipv6-addressing#link-local-address-lla
*/
void printMeshLocalEid(otInstance *aInstance)
{
  const otIp6Address *meshLocalEid = otThreadGetMeshLocalEid(aInstance);

  char mleidString[OT_IP6_ADDRESS_STRING_SIZE];
  EmptyMemory(mleidString, OT_IP6_ADDRESS_STRING_SIZE);

  otIp6AddressToString(meshLocalEid, mleidString, OT_IP6_ADDRESS_STRING_SIZE);

  otLogNotePlat("The Mesh Local EID is %s.", mleidString);
  return;
}
