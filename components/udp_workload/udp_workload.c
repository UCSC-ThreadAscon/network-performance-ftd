#include "udp_workload.h"
#include "inline.h"

/**
 * Creates a UDP socket for the client to send packets to the server
 * with the address defined by the macro `CONFIG_SERVER_IP_ADDRESS`.
 */
void udpCreateSocket(otUdpSocket *socket,
                     otSockAddr *sockAddr)
{
  sockAddr->mAddress = *otThreadGetMeshLocalEid(OT_INSTANCE);
  sockAddr->mPort = UDP_SOURCE_PORT;

  handleError(otUdpOpen(OT_INSTANCE, socket, NULL, NULL), "Failed to create UDP socket.");

  char *errorString = sprintf("Failed to bind MLEID and port %d to socket.",
                              sockAddr->mPort);
  handleError(otUdpBind(OT_INSTANCE, socket, sockAddr, OT_NETIF_THREAD), errorString);
  return;
}
