#ifndef TRANSPORT_TRACE_H
#define TRANSPORT_TRACE_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Fixed RAM, no allocation or serial logging. Retain the first long stall or
 * overflow across core recovery. A debugger may set clear_request=1 to arm
 * a new capture; network_service applies it outside the timed callback. */
enum {
  TRACE_TX_QUEUED=1, TRACE_RX_CALLBACK, TRACE_RX_PAYLOAD,
  TRACE_DELIVERED, TRACE_CREDIT_SENT, TRACE_CREDIT_RECEIVED,
  TRACE_RETURN_ADMITTED, TRACE_RETURN_SENT, TRACE_UART_OVERFLOW,
  TRACE_CREDIT_STALL, TRACE_RETURN_STALL
};
#define TRANSPORT_TRACE_MAGIC 0x54524331U
#define TRANSPORT_TRACE_RECORDS 8U
typedef struct {
  uint32_t magic, frozen, head;
  uint32_t stage_tick[6], stage_total[6];
  uint32_t record[TRANSPORT_TRACE_RECORDS][4]; /* tick,event,total,detail */
} transport_trace_t;
__attribute__((section(".noinit"))) volatile transport_trace_t bridge_transport_trace;
volatile uint32_t bridge_trace_clear_request;
static void bridge_trace_init(void)
{
  uint32_t state=__get_PRIMASK();__disable_irq();
  if(bridge_transport_trace.magic!=TRANSPORT_TRACE_MAGIC || bridge_trace_clear_request) {
    memset((void *)&bridge_transport_trace,0,sizeof(bridge_transport_trace));
    bridge_transport_trace.magic=TRANSPORT_TRACE_MAGIC;
    bridge_trace_clear_request=0;
  }
  __set_PRIMASK(state);
}
static void bridge_trace_event(uint32_t event,uint32_t total,uint32_t detail,bool freeze)
{
  uint32_t state=__get_PRIMASK();__disable_irq();
  if(bridge_transport_trace.magic==TRANSPORT_TRACE_MAGIC && !bridge_transport_trace.frozen) {
    uint32_t now=osKernelGetTickCount();
    if(event>=1U && event<=6U) {
      bridge_transport_trace.stage_tick[event-1U]=now;
      bridge_transport_trace.stage_total[event-1U]=total;
    }
    uint32_t slot=bridge_transport_trace.head%TRANSPORT_TRACE_RECORDS;
    bridge_transport_trace.record[slot][0]=now;
    bridge_transport_trace.record[slot][1]=event;
    bridge_transport_trace.record[slot][2]=total;
    bridge_transport_trace.record[slot][3]=detail;
    bridge_transport_trace.head++;
    __DMB();
    if(freeze)bridge_transport_trace.frozen=1U;
  }
  __set_PRIMASK(state);
}
#endif
