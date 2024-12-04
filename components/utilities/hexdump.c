#include "hexdump.h"
#include "coap_workload.h"

void printHexDump(otLogHexDumpInfo *aInfo) {
  otError error = OT_ERROR_NONE;

  while (error == OT_ERROR_NONE) {
    error = otLogGenerateNextHexDumpLine(aInfo);

    if (error == OT_ERROR_NONE) {
      otLogNotePlat("%s", aInfo->mLine);
    }
    else if (error != OT_ERROR_NOT_FOUND) {
      otLogCritPlat(otThreadErrorToString(error));
    }
  }

  return;
}

void hexDump(void *data, uint16_t dataLength, const char* title) {
  otLogHexDumpInfo hexDump;
  EmptyMemory(&hexDump, sizeof(otLogHexDumpInfo));

  hexDump.mDataBytes = (uint8_t *) data;
  hexDump.mDataLength = dataLength;
  hexDump.mTitle = title;

  printHexDump(&hexDump);
  return;
}