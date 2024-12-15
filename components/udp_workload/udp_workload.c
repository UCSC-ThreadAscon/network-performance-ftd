#include "udp_workload.h"
#include "inline.h"

/**
 * Creates a UDP socket for the client to send packets to the server
 * with the address defined by the macro `CONFIG_SERVER_IP_ADDRESS`.
 */
void udpCreateSocket(otUdpSocket *socket,
                     otSockAddr *sockAddr)
{
  if (!otUdpIsOpen(OT_INSTANCE, socket)) {
    handleError(otUdpOpen(OT_INSTANCE, socket, NULL, NULL), "Failed to create UDP socket.");

    // sockAddr->mAddress = *otThreadGetMeshLocalEid(OT_INSTANCE);
    // sockAddr->mPort = UDP_SOURCE_PORT;
    // handleError(otUdpBind(OT_INSTANCE, socket, sockAddr, OT_NETIF_THREAD),
    //             "Failed to bind MLEID and UDP port to socket.");

    // otLogNotePlat("Created UDP Socket at port %d.", sockAddr->mPort);    
  }
  else {
    otLogNotePlat("UDP Socket is already open.");
  }

  return;
}

void udpSend(otUdpSocket *socket, void* payload, uint16_t payloadLength)
{
  otMessageInfo aMessageInfo;
  aMessageInfo.mSockAddr = socket->mSockName.mAddress;
  aMessageInfo.mSockPort = socket->mSockName.mPort;
  aMessageInfo.mPeerPort = UDP_DEST_PORT;
  aMessageInfo.mHopLimit = 0;  // default

  otIp6Address *peerAddr = &(aMessageInfo.mPeerAddr);
  handleError(otIp6AddressFromString(CONFIG_SERVER_IP_ADDRESS, peerAddr),
              "Failed to parse UDP server IP address string to bytes.");

  otMessage *aMessage = otUdpNewMessage(OT_INSTANCE, NULL);

  otError error = otMessageAppend(aMessage, payload, payloadLength);
  HandleMessageError("Failed to append payload to UDP message.", aMessage, error);

  error = otUdpSend(OT_INSTANCE, socket, aMessage, &aMessageInfo);
  HandleMessageError("Failed to send UDP message", aMessage, error);
  return;
}
