#include <openthread/udp.h>
#include <openthread/logging.h>
#include <openthread/thread.h>

#define UDP_SOURCE_PORT 12345
#define UDP_DEST_PORT 12345

void udpCreateSocket(otUdpSocket *socket, otSockAddr *sockAddr);
void udpSend(otUdpSocket *socket, void* payload, uint16_t payloadLength);
