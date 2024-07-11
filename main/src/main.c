#include "main.h"
#include "tight_loop.h"
#include "independent_variables.h"

void app_main(void)
{
  startMain();

  PrintDelimiter();
  printCipherSuite();
  PrintDelimiter();

  checkConnection(OT_INSTANCE);

#if (DELAY_SERVER || DELAY_CLIENT)
  otNetworkTimeSyncSetCallback(OT_INSTANCE, networkTimeSyncCallbback, NULL);
#endif

/**
 * Delay CoAP server will be started using the "exp-server-start" command
 * through the use of the CLI. Only clients will start their CoAP service
 * automatically.
 */
#if !DELAY_SERVER
  coapStart();
#endif

#if THROUGHPUT_CONFIRMABLE
  tpConfirmableMain();
#elif THROUGHPUT_NONCONFIRMABLE
  tpNonConfirmableMain();
#elif PACKET_LOSS_CONFIRMABLE
  plConfirmableMain();
#elif PACKET_LOSS_NONCONFIRMABLE
  plNonConfirmableMain();
#elif DELAY_CLIENT
  delayConfirmableMain();
#endif

  return;
}
