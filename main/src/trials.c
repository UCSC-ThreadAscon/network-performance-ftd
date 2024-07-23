#include "main.h"

#define MAX_TRIALS 100

/**
 * Records the number of trials done so far in the Delay experiment,
 * and restarts the device after each trial. After NUM_TRIALS trials have
 * been done, the device stops restarting.
 */
void startNextTrial()
{
  switch (esp_reset_reason())
  {
    case ESP_RST_POWERON:
      break;

    case ESP_RST_SW:
      esp_restart();
      break;
    
    default:
      /**
       * This branch should ideally NEVER execute. If it has, a problem that
       * occured during the experiment that resulted in an abnormal reset.
       */
      otLogCritPlat("An abnormal reset has occured during this experimental trial.");
      otLogCritPlat("The experimental trials will need to be rerun.");
  }

  return;
}