#include "main.h"

void networkTimeSyncCallback(void *aCallbackContext)
{
  otLogNotePlat("The Network Time state has changed.");
  return;
}