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

  /**
   * TODO:
   *  Remove checkConnection(). You don't need it. Your clients should
   *  start as soon as they are attached to the Thread network by
   *  using a callback function which is set using `otSetStateChangeCallback()`.
   */
  checkConnection(OT_INSTANCE);

#if (DELAY_SERVER || DELAY_CLIENT)
  otNetworkTimeSyncSetCallback(OT_INSTANCE, networkTimeSyncCallback, NULL);
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
#endif

  return;
}
