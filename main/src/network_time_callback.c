#include "main.h"

void networkTimeSyncCallback(void *aCallbackContext)
{
  otLogNotePlat("The Network Time has now been synchronized.");
  return;
}