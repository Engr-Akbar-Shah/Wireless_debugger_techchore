/* Pico auto-reset DTR/RTS mapping and settle filter (native). */
#include "bridge_engine.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t kind,sent[PC_MAX_DATA];static uint16_t request_id;
bool bridge_port_send(uint8_t k,uint16_t id,const uint8_t *p,unsigned n){kind=k;request_id=id;if(n)memcpy(sent,p,n);return true;}
unsigned bridge_usb_read(uint8_t *p,unsigned n){(void)p;(void)n;return 0;}
unsigned bridge_usb_write(const uint8_t *p,unsigned n){(void)p;return n;}
void bridge_usb_discard_output(void){}
void bridge_usb_flush(void){}
void bridge_log(const char *f,...){(void)f;}
static void lines(uint32_t t,bool dtr,bool rts){bridge.now=t;bridge_usb_lines(dtr,rts);}
/* Step the engine and acknowledge whatever it requested, as A/B would. */
static uint8_t seen[32];static unsigned seen_n;
static void pump(uint32_t from,uint32_t to)
{
  for(uint32_t t=from;t<to;t++) {
    kind=0;bridge_step(t);
    if(kind==PC_QUERY || kind==PC_SET_BAUD) {
      uint32_t line=kind==PC_QUERY ? bridge.desired_line : pc_get32(sent);
      uint8_t p[9]={0};pc_put32(p+1,line);pc_put32(p+5,(line&PC_LINE_EVEN) ? line&PC_LINE_BAUD_MASK : line+line/100);
      assert(bridge_packet(PC_STATUS,request_id,p,9));
    } else if(kind==PC_MODEM) {
      if(seen_n<sizeof(seen))seen[seen_n++]=sent[0];
      uint8_t p[2]={0,sent[0]};assert(bridge_packet(PC_MODEM_ACK,request_id,p,2));
    }
  }
}
int main(void)
{
  bridge_init();bridge_mounted(true);
  /* Terminal open: both asserted -> target keeps running. */
  lines(0,true,false);lines(0,true,true);
  bridge_step(5);assert(bridge.desired_modem==0);          /* not settled yet */
  bridge_step(20);assert(bridge.desired_modem==0);         /* DTR=RTS: run */
  /* RTS alone: hold reset. */
  lines(30,false,true);bridge_step(41);assert(bridge.desired_modem==1);
  /* reset -> boot through a transient DTR=RTS=1 set by two calls. */
  lines(50,true,true);lines(50,true,false);bridge_step(55);assert(bridge.desired_modem==1);
  bridge_step(61);assert(bridge.desired_modem==2);         /* BOOT0 high, reset released */
  /* Back to equal lines: BOOT0 low, running (ROM stays until next reset). */
  lines(70,false,false);bridge_step(81);assert(bridge.desired_modem==0);
  /* Brief glitch shorter than the settle time is ignored. */
  lines(90,false,true);bridge_step(92);lines(93,false,false);bridge_step(120);assert(bridge.desired_modem==0);
  /* USB unplug clears everything. */
  lines(130,false,true);bridge_step(141);assert(bridge.desired_modem==1);
  bridge_mounted(false);assert(bridge.desired_modem==0);
  /* Parity-selected bootloader: CubeProgrammer opens 8E1, terminal 8N1. */
  bridge_init();bridge_mounted(true);lines(200,true,true);pump(200,260);
  assert(bridge.synced && bridge.applied_modem==0 && !bridge.auto_boot);
  seen_n=0;bridge.now=300;bridge_line_changed(115200,8,2,0);pump(300,360);
  assert(seen_n==2 && seen[0]==1 && seen[1]==2);             /* reset, then BOOT0 with reset released */
  assert(bridge.auto_boot==2 && bridge.applied_modem==2 && (bridge.applied_line&PC_LINE_EVEN));
  lines(370,false,true);pump(370,400);assert(bridge.applied_modem==2); /* tool's own lines ignored in ROM */
  seen_n=0;bridge.now=400;bridge_line_changed(115200,8,0,0);lines(400,true,true);pump(400,460);
  assert(seen_n==2 && seen[0]==1 && seen[1]==0);             /* reset, then run with BOOT0 low */
  assert(!bridge.auto_boot && bridge.applied_modem==0 && !(bridge.applied_line&PC_LINE_EVEN));
  puts("auto bootloader on 8E1 / application on 8N1: all checks passed");
  puts("modem mapping: all checks passed");
  return 0;
}
