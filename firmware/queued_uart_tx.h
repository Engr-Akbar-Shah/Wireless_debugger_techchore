#ifndef QUEUED_UART_TX_H
#define QUEUED_UART_TX_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "cmsis_os2.h"
#include "USART.h"

/* One network producer, one UART worker. Credits mean accepted into this
 * bounded queue, not that the final stop bit has already left the pin.
 * The worker wakes immediately on DMA completion rather than polling each
 * packet with osDelay(1). DMA scratch must reside in the UART's memory domain. */
/* RX/TX memory is budgeted together with the RTOS heap. The recorded
 * minimum RTOS headroom permits its50KiB heap to shrink to32KiB; the
 * released18KiB funds the enlarged network/UART receive queues while
 * preserving the SDK48480-byte C-heap packet pool. The later measured
 * C-heap budget uses28KiB RTOS memory, retaining at least7.9KiB of its
 * observed minimum headroom on B and funding4KiB more C allocation space. */
#define QUEUED_UART_SIZE 16384U
#define QUEUED_UART_BLOCK 1024U
#define QUEUED_UART_KICK 1U
#define QUEUED_UART_DONE 2U
volatile uint32_t queued_uart_chained_blocks, queued_uart_initial_blocks;
volatile uint32_t queued_uart_write_timeouts;
volatile uint32_t queued_uart_unprepared_completions;
volatile uint32_t queued_uart_setup_max_timer_ticks, queued_uart_timer_hz;
volatile uint32_t queued_uart_setup_total_lo, queued_uart_setup_total_hi;
volatile uint32_t queued_uart_full_blocks, queued_uart_full_cycles_lo, queued_uart_full_cycles_hi;
volatile uint32_t queued_uart_dma_max_cycles, queued_uart_chain_idle;
volatile uint32_t queued_uart_chain_fifo_min = 0xffffffffU;
static void queued_uart_add_cycles(volatile uint32_t *lo, volatile uint32_t *hi, uint32_t cycles)
{
  uint32_t old = *lo; *lo = old + cycles; if (*lo < old) (*hi)++;
}
typedef struct {
  uint8_t data[QUEUED_UART_SIZE];
  volatile uint32_t head, tail;
  volatile bool failed;
  ARM_DRIVER_USART *driver;
  USART0_Type *registers;
  uint32_t started_cycles;
  uint8_t *scratch;
  osThreadId_t worker;
  volatile uint32_t *sent, *hash, *errors, *peak;
  volatile uint32_t active_count, active_slot, active_hash;
  volatile uint32_t prepared_count, prepared_slot, prepared_hash;
  volatile uint32_t generation;
} queued_uart_tx_t;

/* Called with interrupts masked. The SDK clears send_active before invoking
 * SEND_COMPLETE, so its existing nonblocking Send can rearm from that callback
 * just as the RX path already rearms Receive from its DMA callback. */
static void queued_uart_start_locked(queued_uart_tx_t *q, uint32_t slot,
                                     uint32_t count, uint32_t hash)
{
  q->active_slot = slot;
  q->active_hash = hash;
  q->active_count = count;
  q->generation++;
  /* Debug-probe detach can clear TRCENA; restore before profiling. */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  uint32_t started = DWT->CYCCNT;
  q->started_cycles = started;
  int32_t status = q->driver->Send(q->scratch + slot * QUEUED_UART_BLOCK, count);
  uint32_t ticks = DWT->CYCCNT - started;
  queued_uart_add_cycles(&queued_uart_setup_total_lo, &queued_uart_setup_total_hi, ticks);
  if (ticks > queued_uart_setup_max_timer_ticks) queued_uart_setup_max_timer_ticks = ticks;
  if (status != ARM_DRIVER_OK) {
    (*q->errors)++;
    q->failed = true;
    q->active_count = 0U;
  }
}

static void queued_uart_worker(void *argument)
{
  queued_uart_tx_t *q = argument;
  while (1) {
    uint32_t irq_state = __get_PRIMASK();
    __disable_irq();
    uint32_t generation = q->generation;
    uint32_t active = q->active_count;
    uint32_t tail = (q->tail + active) % QUEUED_UART_SIZE;
    uint32_t slot = active ? q->active_slot ^ 1U : 0U;
    uint32_t hash = active ? q->active_hash : *q->hash;
    bool prepare = !q->failed && !q->prepared_count;
    __set_PRIMASK(irq_state);
    uint32_t count = (q->head + QUEUED_UART_SIZE - tail) % QUEUED_UART_SIZE;
    if (!prepare || !count) {
      uint32_t flags = osThreadFlagsWait(QUEUED_UART_KICK | QUEUED_UART_DONE,
                         osFlagsWaitAny, active ? osKernelGetTickFreq() : osWaitForever);
      if (flags == osFlagsErrorTimeout && active) {
        irq_state = __get_PRIMASK();
        __disable_irq();
        if (q->generation == generation && q->active_count) {
          q->driver->Control(ARM_USART_ABORT_SEND, 0U);
          (*q->errors)++;
          q->failed = true;
          q->active_count = q->prepared_count = 0U;
          q->generation++;
        }
        __set_PRIMASK(irq_state);
      }
      continue;
    }
    if (count > QUEUED_UART_BLOCK) count = QUEUED_UART_BLOCK;
    uint32_t first = QUEUED_UART_SIZE - tail;
    if (first > count) first = count;
    uint8_t *scratch = q->scratch + slot * QUEUED_UART_BLOCK;
    memcpy(scratch, q->data + tail, first);
    if (count > first) memcpy(scratch + first, q->data, count - first);
    /* Copy/hash the NEXT buffer while the CURRENT DMA transfer runs.
     * Completion only publishes counters and starts the prepared buffer. */
    for (uint32_t i = 0; i < count; ++i) hash = (hash ^ scratch[i]) * 16777619U;
    irq_state = __get_PRIMASK();
    __disable_irq();
    if (!q->failed) {
      if (generation == q->generation && active) {
        q->prepared_slot = slot;
        q->prepared_hash = hash;
        __DMB();
        q->prepared_count = count;
      } else if (!q->active_count && q->tail == tail) {
        /* DMA may have completed while this buffer was being prepared. */
        queued_uart_initial_blocks++;
        queued_uart_start_locked(q, slot, count, hash);
      }
    }
    __set_PRIMASK(irq_state);
  }
}

static bool queued_uart_init(queued_uart_tx_t *q, ARM_DRIVER_USART *driver,
                             uint8_t *scratch, USART0_Type *registers, volatile uint32_t *sent,
                             volatile uint32_t *hash, volatile uint32_t *errors,
                             volatile uint32_t *peak)
{
  q->driver = driver; q->scratch = scratch; q->registers = registers;
  q->sent = sent; q->hash = hash; q->errors = errors; q->peak = peak;
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  queued_uart_timer_hz = SystemCoreClock;
  const osThreadAttr_t attributes = {
    .name = "uart_tx", .stack_size = 1024, .priority = osPriorityHigh
  };
  q->worker = osThreadNew(queued_uart_worker, q, &attributes);
  return q->worker != NULL;
}

static void queued_uart_complete(queued_uart_tx_t *q)
{
  uint32_t irq_state = __get_PRIMASK();
  __disable_irq();
  uint32_t count = q->active_count;
  if (count) {
    uint32_t cycles = DWT->CYCCNT - q->started_cycles;
    if (cycles > queued_uart_dma_max_cycles) queued_uart_dma_max_cycles = cycles;
    if (count == QUEUED_UART_BLOCK) {
      queued_uart_full_blocks++;
      queued_uart_add_cycles(&queued_uart_full_cycles_lo, &queued_uart_full_cycles_hi, cycles);
    }
    *q->sent += count;
    *q->hash = q->active_hash;
    __DMB();
    q->tail = (q->tail + count) % QUEUED_UART_SIZE;
    q->active_count = 0U;
    q->generation++;
    uint32_t prepared = q->prepared_count;
    if (prepared) {
      uint32_t slot = q->prepared_slot, hash = q->prepared_hash;
      q->prepared_count = 0U;
      queued_uart_chained_blocks++;
      uint32_t fifo = q->registers->TFL;
      if (fifo < queued_uart_chain_fifo_min) queued_uart_chain_fifo_min = fifo;
      if (q->registers->LSR & (1U << 6)) queued_uart_chain_idle++;
      queued_uart_start_locked(q, slot, prepared, hash);
    } else if (q->head != q->tail) {
      queued_uart_unprepared_completions++;
    }
  }
  __set_PRIMASK(irq_state);
  if (q->worker) osThreadFlagsSet(q->worker, QUEUED_UART_DONE);
}

static bool queued_uart_write(queued_uart_tx_t *q, const uint8_t *data, uint32_t length)
{
  if (!length) return true;
  if (length >= QUEUED_UART_SIZE || !q->worker) return false;
  uint32_t started = osKernelGetTickCount();
  uint32_t head = q->head;
  while ((q->tail + QUEUED_UART_SIZE - head - 1U) % QUEUED_UART_SIZE < length) {
    if (q->failed) return false;
    if ((uint32_t)(osKernelGetTickCount() - started) > 2U * osKernelGetTickFreq()) {
      queued_uart_write_timeouts++; (*q->errors)++; return false;
    }
    osDelay(1U);
  }
  if (q->failed) return false;
  uint32_t first = QUEUED_UART_SIZE - head;
  if (first > length) first = length;
  memcpy(q->data + head, data, first);
  if (length > first) memcpy(q->data, data + first, length - first);
  __DMB();
  q->head = (head + length) % QUEUED_UART_SIZE;
  uint32_t pending = (q->head + QUEUED_UART_SIZE - q->tail) % QUEUED_UART_SIZE;
  if (pending > *q->peak) *q->peak = pending;
  osThreadFlagsSet(q->worker, QUEUED_UART_KICK);
  return true;
}
#endif
