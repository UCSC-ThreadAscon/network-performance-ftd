/**
 * The UDP API is based on the example code given in the OpenThread source code:
 * https://github.com/openthread/openthread/blob/main/src/cli/cli_udp.cpp
 * https://github.com/jwhui/openthread/blob/405de3cb14c62c56faa79ecde45e16e29a7ecb15/src/cli/cli_udp_example.cpp
 *
 * In addition, I got the idea to use `otUdpConnect()` from:
 * https://groups.google.com/g/openthread-users/c/7PAiNorSnBg/m/m3yqM9sRCgAJ
 */
#include "udp_workload.h"
#include "inline.h"

/**
 * Creates a UDP socket for the client to send packets to the server
 * with the address defined by the macro `CONFIG_SERVER_IP_ADDRESS`.
 */
void udpCreateSocket(otUdpSocket *socket,
                     otSockAddr *destAddr)
{
  if (!otUdpIsOpen(OT_INSTANCE, socket)) {
    handleError(otUdpOpen(OT_INSTANCE, socket, NULL, NULL), "Failed to create UDP socket.");

  otIp6Address destIp;
  EmptyMemory(&destIp, sizeof(otIp6Address));
  handleError(otIp6AddressFromString(CONFIG_SERVER_IP_ADDRESS, &destIp),
              "Failed to parse UDP server IP address string to bytes.");

  destAddr->mAddress = destIp;
  destAddr->mPort = UDP_DEST_PORT;
  handleError(otUdpConnect(OT_INSTANCE, socket, destAddr),
              "Failed to connect UDP socket to server.");

  otLogNotePlat("Connected to UDP Server at port %d.", destAddr->mPort);    
  }
  else {
    otLogWarnPlat("UDP Socket is already open.");
  }

  return;
}

void udpSend(otUdpSocket *socket, void* payload, uint16_t payloadLength)
{
  otMessageInfo aMessageInfo;
  EmptyMemory(&aMessageInfo, sizeof(otMessageInfo));

  otMessage *aMessage = otUdpNewMessage(OT_INSTANCE, NULL);

  otError error = otMessageAppend(aMessage, payload, payloadLength);
  HandleMessageError("Failed to append payload to UDP message.", aMessage, error);

  error = otUdpSend(OT_INSTANCE, socket, aMessage, &aMessageInfo);
  HandleMessageError("Failed to send UDP message", aMessage, error);

#if CONFIG_EXPERIMENT_DEBUG
  otLogNotePlat("Sent a UDP packet with a length of %" PRIu16 " bytes.", payloadLength);
#endif
  return;
}
