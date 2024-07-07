#include "tight_loop.h"

void createRandomPayload(uint8_t *buffer) {
  uint32_t random = esp_random();
  memcpy(buffer, &random, TIGHT_LOOP_PAYLOAD_BYTES);
  return;
}