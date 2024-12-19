#include "uuid.h"

void generateUUID(uuid *uuid) {
  uint32_t *uuidWords = (uint32_t *) uuid->bytes;

  for (int i = 0; i < UUID_SIZE_WORDS; i++)
  {
    uuidWords[i] = esp_random();
  }
  return;
}

#define ONE_BYTE_HEX_STRING sizeof(char) * 2

void printUUID(uuid *uuid) {
  hexDump(uuid->bytes, UUID_SIZE_BYTES, "Sleepy End Device UUID");
  return;
}