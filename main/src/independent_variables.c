#include "independent_variables.h"

void printCipherSuite()
{
#if (CIPHER_SUITE == AES)
  otLogNotePlat("Cipher Suite: AES");
#elif (CIPHER_SUITE == NONE)
  otLogNotePlat("Cipher Suite: No Encryption");
#elif (CIPHER_SUITE == LIBASCON_128A)
  otLogNotePlat("Cipher Suite: ASCON-128a");
#elif (CIPHER_SUITE == LIBASCON_128)
  otLogNotePlat("Cipher Suite: ASCON-128");
#endif
  return;
}

void printTxPower()
{
  int8_t txPower = 0;
  if (getTxPower(&txPower) != OT_ERROR_NONE)
  {
    otLogCritPlat("Failed to get TX Power.");
  }
  return;
}

void printNetPerfTest()
{
#if THROUGHPUT_CONFIRMABLE
  otLogNotePlat("Current Test: Throughput Confirmable");
#elif THROUGHPUT_NONCONFIRMABLE
  otLogNotePlat("Current Test: Throughput NON-Confirmable");
#elif PACKET_LOSS_CONFIRMABLE
  otLogNotePlat("Current Test: Packet Loss Confirmable");
#elif PACKET_LOSS_NONCONFIRMABLE
  otLogNotePlat("Current Test: Packet Loss NON-Confirmable");
#elif DELAY_CLIENT
  otLogNotePlat("Current Test: Delay Client (Confirmable)");
#elif DELAY_SERVER
  otLogNotePlat("Current Test: Delay Server (Confirmable)");
#endif
  return;
}

void printTimeSyncStatus()
{
#if CONFIG_OPENTHREAD_TIME_SYNC
  otLogNotePlat("Time Sync is ON.");
#else
  otLogNotePlat("Time Sync is OFF.");
#endif
  return;
}