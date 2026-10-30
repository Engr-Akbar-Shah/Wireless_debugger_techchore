#ifndef DIAGNOSTIC_SOURCE_H
#define DIAGNOSTIC_SOURCE_H
#include <stdint.h>
/* Bench-only mailbox, disabled at boot.1=return source,2=source+forward sink.
 * Source follows elapsed time, not delivery credits, like a continuous UART. */
extern volatile uint32_t bridge_diagnostic_request,bridge_diagnostic_mode;
extern volatile uint32_t bridge_diagnostic_goal,bridge_diagnostic_produced;
extern volatile uint32_t bridge_diagnostic_started,bridge_diagnostic_finished;
extern volatile uint32_t bridge_diagnostic_error;
#ifdef BRIDGE_DIAGNOSTIC_SOURCE_IMPLEMENTATION
volatile uint32_t bridge_diagnostic_request,bridge_diagnostic_mode;
volatile uint32_t bridge_diagnostic_goal=2764800U,bridge_diagnostic_produced;
volatile uint32_t bridge_diagnostic_started,bridge_diagnostic_finished,bridge_diagnostic_error;
static uint32_t diagnostic_rng;
static void diagnostic_source_service(uint32_t now,uint32_t hz,
    void (*emit)(const uint8_t *,uint32_t))
{
  if(bridge_diagnostic_request) {
    bridge_diagnostic_mode=bridge_diagnostic_request;bridge_diagnostic_request=0;
    bridge_diagnostic_produced=bridge_diagnostic_finished=bridge_diagnostic_error=0;
    diagnostic_rng=917U;bridge_diagnostic_started=now+hz;
  }
  if(!bridge_diagnostic_mode || bridge_diagnostic_error || bridge_diagnostic_finished
      || (int32_t)(now-bridge_diagnostic_started)<0)return;
  uint32_t due=(uint32_t)(((uint64_t)(now-bridge_diagnostic_started)*92160U)/hz);
  if(due>bridge_diagnostic_goal)due=bridge_diagnostic_goal;
  uint8_t packet[256];
  for(unsigned budget=0;budget<4 && due-bridge_diagnostic_produced>=256U;budget++) {
    for(unsigned i=0;i<sizeof(packet);i++) {
      if(!(i&3U)) {diagnostic_rng^=diagnostic_rng<<13;diagnostic_rng^=diagnostic_rng>>17;diagnostic_rng^=diagnostic_rng<<5;}
      packet[i]=(uint8_t)(diagnostic_rng>>(8U*(i&3U)));
    }
    emit(packet,sizeof(packet));bridge_diagnostic_produced+=sizeof(packet);
    if(bridge_diagnostic_error)break;
  }
  if(bridge_diagnostic_produced==bridge_diagnostic_goal)bridge_diagnostic_finished=now;
}
#endif
#endif
