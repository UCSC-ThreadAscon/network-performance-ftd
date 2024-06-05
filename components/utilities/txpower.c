#include "utilities.h"

#include <openthread/thread.h>
#include <openthread/platform/radio.h>

#include <assert.h>

#define UnhandledError()                                        \
    otLogCritPlat("Cannot Handle Error: %" PRIu8 ",", error)    \

otError getTxPower(int8_t *aPowerAddr)
{
  otError error = otPlatRadioGetTransmitPower(esp_openthread_get_instance(),
                                              aPowerAddr);

  switch (error)
  {
    case OT_ERROR_NOT_IMPLEMENTED:
      otLogCritPlat("Functionality for TX Power is not yet implemented.");
      break;

    case OT_ERROR_INVALID_ARGS:
      otLogCritPlat("Failed to get TX power: &aPowerAddr is NULL.");
      break;

    case OT_ERROR_NONE:
      otLogNotePlat("Max TX Power is: %" PRId8 " dBm", *aPowerAddr);
      break;

    default:
      UnhandledError();
  }
  return error;
}

#define GetTxPowerOrExit(aPowerAddr)                        \
  if (getTxPower(aPowerAddr) != OT_ERROR_NONE) {            \
    otLogCritPlat("Failed to get TX Power.");               \
  }                                                         \

void setTxPower() {
  otError error = otPlatRadioSetTransmitPower(esp_openthread_get_instance(),
                                              CONFIG_TX_POWER);
  switch(error)
  {
    case (OT_ERROR_NOT_IMPLEMENTED):
      otLogCritPlat("Functionality for setting TX power not yet implemented.");
      break;

    case (OT_ERROR_NONE):
#if TX_POWER_DEBUG
      otLogNotePlat("Successfully set TX Power.");
#endif
      break;

    default:
        UnhandledError();
  }

  int8_t currentPower; GetTxPowerOrExit(&currentPower);
  return;
}
