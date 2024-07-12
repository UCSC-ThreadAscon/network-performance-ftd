#include "delay_server.h"
#include "delay.h"
#include "main.h"

static otCoapResource *experimentRoute;

void startCoapServer(uint16_t port) {
  otError error = otCoapStart(OT_INSTANCE, port);

  if (error != OT_ERROR_NONE) {
    otLogCritPlat("Failed to start COAP server.");
  } else {
    otLogNotePlat("Started CoAP server at port %d.", port);
  }
  return;
}

otError expServerStart(void* aContext, uint8_t argsLength, char* aArgs[]) 
{
  checkConnection(OT_INSTANCE);
  startCoapServer(OT_DEFAULT_COAP_PORT);

  experimentRoute = calloc(1, sizeof(otCoapResource));
  createResource(experimentRoute, "Delay Confirmable", delayRequestHandler);

  return OT_ERROR_NONE;
}

otError expServerFree(void* aContext, uint8_t argsLength, char* aArgs[])
{
  resourceDestructor(experimentRoute);
  otCoapStop(OT_INSTANCE);
  return OT_ERROR_NONE;
}