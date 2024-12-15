#include "udp_workload.h"
#include "inline.h"

/**
 * Creates a UDP socket for the client to send packets to the server
 * with the address defined by the macro `CONFIG_SERVER_IP_ADDRESS`.
 */
void udpCreateSocket(otUdpSocket *socket)
{
  socket->mSockName.mAddress = *otThreadGetMeshLocalEid(OT_INSTANCE);
  socket->mSockName.mPort = UDP_SOURCE_PORT;

  return;
}
