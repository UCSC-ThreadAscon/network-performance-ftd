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
  return;
}
