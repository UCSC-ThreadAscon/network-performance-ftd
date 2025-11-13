#include "independent_variables.h"

void printCipherSuite()
{
#if (CIPHER_SUITE == AES)
  otLogNotePlat("Cipher Suite: AES");
#elif (CIPHER_SUITE == NONE)
  otLogNotePlat("Cipher Suite: No Encryption");
#elif (CIPHER_SUITE == LIBASCON_128A)
  otLogNotePlat("Cipher Suite: ASCON-128a (LibAscon)");
#elif (CIPHER_SUITE == LIBASCON_128)
  otLogNotePlat("Cipher Suite: ASCON-128 (LibAscon)");
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
#elif RTT_CPU_CONFIRMABLE
  otLogNotePlat("Current Test: RTT Confirmable & CPU Usage");
#elif PACKET_LOSS_CONFIRMABLE
  otLogNotePlat("Current Test: Packet Loss Confirmable");
#elif THROUGHPUT_OBSERVE
  otLogNotePlat("Current Test: Throughput Observe (Non-Confirmable)");
#elif PACKET_LOSS_OBSERVE
  otLogNotePlat("Current Test: Packet Loss Observe (Non-Confirmable)");
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
  otLogNotePlat("Network Time Sync is ON.");
#else
  otLogNotePlat("Time Sync is OFF.");
#endif
  return;
}

void printTimeSyncPeriod()
{
#if CONFIG_OPENTHREAD_TIME_SYNC
  uint16_t timeSyncPeriod = otNetworkTimeGetSyncPeriod(esp_openthread_get_instance());
  otLogNotePlat("The Time Sync Period is %" PRIu16 " seconds.", timeSyncPeriod);
#endif
  return;
}

void printNetworkKey()
{
  otNetworkKey key;
  EmptyMemory(&key, sizeof(otNetworkKey));

  otThreadGetNetworkKey(esp_openthread_get_instance(), &key);
  otDumpNotePlat("Thread Network Key:", key.m8, OT_NETWORK_KEY_SIZE);
  return;
}