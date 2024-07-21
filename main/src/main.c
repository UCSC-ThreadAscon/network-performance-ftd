#include "main.h"
#include "tight_loop.h"
#include "independent_variables.h"

#if ((DELAY_SERVER || DELAY_CLIENT) && !CONFIG_OPENTHREAD_TIME_SYNC)
#error "You must TURN ON Time Sync for the Delay experiments."
#elif (!(DELAY_SERVER || DELAY_CLIENT) && CONFIG_OPENTHREAD_TIME_SYNC)
#error "You must TURN OFF Time Sync for the Throughput & Packet Loss experiments."
#endif

void app_main(void)
{
  startMain();

  PrintDelimiter();
  printCipherSuite();
  printTxPower();
  printNetPerfTest();
  printTimeSyncStatus();
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
#elif PACKET_LOSS_CONFIRMABLE
  plConfirmableMain();
#elif DELAY_CLIENT
  delayConfirmableMain();
#endif

  return;
}
