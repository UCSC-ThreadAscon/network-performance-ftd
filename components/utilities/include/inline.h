#pragma once

#include "dependencies.h"

#define OT_INSTANCE esp_openthread_get_instance()

#define MS_TO_TICKS(ms) ms / portTICK_PERIOD_MS
#define MS_TO_MICRO(ms) ms * 1000

#define handleError(error, desc)                                        \
  if (error != OT_ERROR_NONE) {                                         \
    otLogCritPlat("%s error: %s", desc, otThreadErrorToString(error));  \
    return;                                                             \
  }                                                                     \

#define HandleMessageError(desc, aMessage, error)       \
  if (error != OT_ERROR_NONE) {                         \
    otMessageFree(aMessage);                            \
    handleError(error, desc);                           \
    return;                                             \
  }                                                     \

/**
 * Empties all memory for `size` bytes starting at memory address `pointer`.
 *
 * @param[in] pointer: the pointer of the stack memory
 * @param[in] size:    the size of the memory that `pointer` points to
 *
 * I got the idea to use `memset()` to clear stack memory from
 * the Google Search AI:
 * https://docs.google.com/document/d/1o-NaEOA-vzWPCv7VX1dONUfwos2epveDk4H_Y2Y5g1Y/edit?usp=sharing
*/
static inline void EmptyMemory(void* pointer, size_t size) {
  memset(pointer, 0, size);
  return;
}

static inline bool connected(otDeviceRole role)
{
  return (role == OT_DEVICE_ROLE_CHILD)  ||
         (role == OT_DEVICE_ROLE_ROUTER) ||
         (role == OT_DEVICE_ROLE_LEADER);
}
