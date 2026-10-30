#ifndef BRIDGE_ALLOCATOR_H
#define BRIDGE_ALLOCATOR_H
#include <stdint.h>
#include <stdlib.h>
#include "cmsis_os2.h"
volatile uint32_t bridge_c_alloc_failures, bridge_c_alloc_last_size, bridge_c_alloc_failure_tick;
extern void *__real_malloc(size_t);
extern void *__real_calloc(size_t, size_t);
extern void *__real_realloc(void *, size_t);
static void bridge_record_allocation_failure(size_t bytes)
{
  bridge_c_alloc_failures++;
  bridge_c_alloc_last_size = (uint32_t)bytes;
  bridge_c_alloc_failure_tick = osKernelGetState() == osKernelRunning ? osKernelGetTickCount() : 0U;
}
void *__wrap_malloc(size_t bytes)
{
  void *p = __real_malloc(bytes);
  if (!p && bytes) bridge_record_allocation_failure(bytes);
  return p;
}
void *__wrap_calloc(size_t count, size_t bytes)
{
  void *p = __real_calloc(count, bytes);
  if (!p && count && bytes) bridge_record_allocation_failure(bytes > SIZE_MAX/count ? SIZE_MAX : bytes*count);
  return p;
}
void *__wrap_realloc(void *old, size_t bytes)
{
  void *p = __real_realloc(old, bytes);
  if (!p && bytes) bridge_record_allocation_failure(bytes);
  return p;
}
#endif
