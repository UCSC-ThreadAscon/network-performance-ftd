#pragma once

#include "dependencies.h"

#define OT_INSTANCE esp_openthread_get_instance()

#define OT_DISCONNECTED(role) (role == OT_DEVICE_ROLE_DISABLED) || (role == OT_DEVICE_ROLE_DETACHED)

#define MS_TO_TICKS(ms) ms / portTICK_PERIOD_MS
#define MS_TO_MICRO(ms) ms * 1000

#define HandleMessageError(desc, aMessage, error)       \
  if (error != OT_ERROR_NONE) {                         \
    otMessageFree(aMessage);                            \
    handleError(error, desc);                           \
    return;                                             \
  }                                                     \

/**
 * Keeps a thread open so the memory associated with "socket" still exists.
*/
#define KEEP_THREAD_ALIVE()                           \
  while (true) { vTaskDelay(THREAD_WAIT_TIME); }      \

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
