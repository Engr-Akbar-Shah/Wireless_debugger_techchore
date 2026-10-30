#include "bridge_engine.h"
#include <string.h>
bridge_engine_t bridge;
enum { WAIT_NONE=0,WAIT_QUERY=1,WAIT_LINE=2,WAIT_MODEM=3 };
static uint16_t next_control(void)
{ bridge.control_id++;if(!bridge.control_id)bridge.control_id++;return bridge.control_id; }
static void session_lost(const char *reason)
{
  bridge_log("FAULT %s; pending session bytes are not preserved\n",reason);
  bridge.synced=false;bridge.fault=true;bridge.waiting=0;bridge.applied_line=0;
  bridge.applied_modem=255;bridge.tx_sequence=bridge.tx_ack=bridge.rx_sequence=0;
  bridge.pending_length=0;bridge.output_tail=bridge.output_head=0;
  bridge.retry_at=bridge.now+500;bridge.errors++;bridge.session_resets++;
  bridge_usb_flush();
}
void bridge_transport_fault(const char *reason) { session_lost(reason); }
void bridge_init(void)
{
  memset(&bridge,0,sizeof(bridge));bridge.desired_line=115200;
  bridge.valid_line=true;bridge.applied_modem=255;
}
void bridge_line_changed(uint32_t baud,uint8_t data,uint8_t parity,uint8_t stop)
{
  bridge.valid_line=pc_baud_supported(baud) && data==8 && (parity==0 || parity==2) && stop==0;
  if(!bridge.valid_line) {
    bridge_log("UNSUPPORTED baud=%lu data=%u parity=%u stop=%u; DATA blocked\n",(unsigned long)baud,data,parity,stop);
    return;
  }
  bool was_even=(bridge.desired_line&PC_LINE_EVEN)!=0;
  bridge.desired_line=baud | (parity==2 ? PC_LINE_EVEN : 0U);
  bridge_log("USB LINE requested baud=%lu parity=%s\n",(unsigned long)baud,parity==2 ? "E" : "N");
  /* The STM32 ROM bootloader only speaks 8E1; terminals use 8N1. Even parity
   * therefore selects the bootloader (reset, BOOT0 high, release) and a return
   * to no parity resets into the application. No DTR/RTS sequence is needed. */
  bool even=parity==2;
  if(even==was_even) return; /* Only a parity change selects the target mode. */
  if(even && (bridge.auto_boot==0 || bridge.auto_boot==3)) {
    bridge.auto_boot=1;bridge_modem_changed(true,false);
    bridge_log("AUTO BOOT: 8E1 selects STM32 bootloader\n");
  } else if(!even && (bridge.auto_boot==1 || bridge.auto_boot==2)) {
    bridge.auto_boot=3;bridge_modem_changed(true,false);
    bridge_log("AUTO BOOT: 8N1 resets STM32 into application\n");
  }
}
/* Advances the automatic sequence once each reset step is acknowledged. */
static void bridge_auto_boot_step(void)
{
  if((bridge.auto_boot!=1 && bridge.auto_boot!=3) || bridge.waiting
     || bridge.applied_modem!=bridge.desired_modem || bridge.applied_line!=bridge.desired_line) return;
  if(bridge.auto_boot==1) { bridge.auto_boot=2;bridge_modem_changed(false,true); }
  else { bridge.auto_boot=0;bridge.usb_lines_used=0xFFU;bridge_modem_changed(false,false); }
}
/* Auto-reset mapping as on ESP32-style USB-UART boards: equal DTR/RTS (every
 * terminal asserts both on open) leaves the target running; RTS alone holds
 * reset, DTR alone selects BOOT0. Programs set the two lines in separate calls,
 * so only a state stable for BRIDGE_MODEM_SETTLE_MS is forwarded: the passing
 * DTR=RTS=1 between "reset" and "boot" must not release a run state. */
void bridge_usb_lines(bool dtr,bool rts)
{
  bridge.usb_lines=(dtr ? 1U : 0U)|(rts ? 2U : 0U);
  bridge.usb_lines_at=bridge.now;
  bridge_log("USB lines dtr=%u rts=%u\n",dtr,rts);
}
static void bridge_settle_lines(void)
{
  uint8_t lines=bridge.usb_lines;
  if(bridge.auto_boot) return; /* Parity-selected boot owns the target controls. */
  if(lines==bridge.usb_lines_used || bridge.now-bridge.usb_lines_at<BRIDGE_MODEM_SETTLE_MS) return;
  bridge.usb_lines_used=lines;
  bool dtr=lines&1U,rts=(lines>>1)&1U;
  bridge_modem_changed(rts && !dtr,dtr && !rts);
}
/* Logical target controls: hold reset, BOOT0 high. */
void bridge_modem_changed(bool dtr,bool rts)
{
  uint8_t prior=bridge.desired_modem;
  if(rts && (!(prior&2U) || dtr)) {
    bridge.boot_sync_pending=true;bridge.boot_prefix_discarded=0;
    bridge.output_tail=bridge.output_head;bridge_usb_discard_output();
  } else if(!rts && (dtr || (prior&1U))) {
    /* Reset held/released with BOOT0 low selects the application. Closing
     * an already released ROM port must not re-open a reset-noise window. */
    bridge.boot_sync_pending=false;
  }
  bridge.desired_modem=(dtr ? 1U : 0U)|(rts ? 2U : 0U);
  bridge_log("USB MODEM requested dtr=%u rts=%u\n",dtr,rts);
}
void bridge_mounted(bool mounted)
{
  bridge.mounted=mounted;
  if(!mounted) bridge.desired_modem=bridge.usb_lines=bridge.usb_lines_used=bridge.auto_boot=0;
  bridge_log("USB %s\n",mounted ? "mounted" : "unmounted; release target controls");
}
bool bridge_ready(void)
{ return bridge.mounted && bridge.synced && bridge.valid_line && !bridge.waiting
    && bridge.applied_line==bridge.desired_line && bridge.applied_modem==bridge.desired_modem && !bridge.fault; }
void bridge_status(void)
{
  bridge_log("STATUS ready=%u baud=%lu parity=%s driver=%lu dtr=%u rts=%u errors=%lu tx=%lu rx=%lu valid=%u synced=%u waiting=%u wanted=%lu batches=%lu full=%lu idle=%lu drain=%lu\n",
    bridge_ready(),(unsigned long)(bridge.applied_line&PC_LINE_BAUD_MASK),
    (bridge.applied_line&PC_LINE_EVEN) ? "E" : "N",(unsigned long)bridge.driver,
    bridge.applied_modem==255 ? 0 : bridge.applied_modem&1U,
    bridge.applied_modem==255 ? 0 : (bridge.applied_modem>>1)&1U,
    (unsigned long)bridge.errors,(unsigned long)bridge.usb_rx_bytes,(unsigned long)bridge.usb_tx_bytes,
    bridge.valid_line,bridge.synced,bridge.waiting,(unsigned long)(bridge.desired_line&PC_LINE_BAUD_MASK),
    (unsigned long)bridge.tx_batches,(unsigned long)bridge.tx_full_batches,
    (unsigned long)bridge.tx_idle_batches,(unsigned long)bridge.tx_config_flush_batches);
}
bool bridge_packet(uint8_t type,uint16_t id,const uint8_t *p,unsigned n)
{
  if(type==PC_PONG) { bridge.last_pong=bridge.now;return true; }
  if(type==PC_DATA_ACK) {
    uint16_t advance=(uint16_t)(id-bridge.tx_ack);
    if(advance>(uint16_t)(bridge.tx_sequence-bridge.tx_ack)) { session_lost("invalid DATA ACK");return true; }
    bridge.tx_ack=id;return true;
  }
  if(type==PC_DATA) {
    if(!bridge.synced) return true; /* Old session bytes cannot qualify a new session. */
    if(id!=(uint16_t)(bridge.rx_sequence+1U)) { session_lost("DATA sequence discontinuity");return true; }
    unsigned skip=0;bool pending=bridge.boot_sync_pending;
    bool rom_line=(bridge.applied_line&PC_LINE_EVEN)!=0;
    uint32_t discarded=bridge.boot_prefix_discarded;
    while(pending && rom_line && skip<n) {
      if(p[skip]==0x79U || p[skip]==0x1fU) {pending=false;break;}
      if(discarded>=512U) {session_lost("ROM reset prefix exceeded512bytes");return true;}
      discarded++;skip++;
    }
    unsigned keep=n-skip;
    unsigned free=BRIDGE_USB_RING-(bridge.output_head-bridge.output_tail);
    if(keep>free) return false;
    /* Prefix scanning is committed only after reserving ACK space, so a
     * backpressure retry cannot expose bytes already examined as reset noise. */
    if(!bridge_port_send(PC_DATA_ACK,id,NULL,0)) return false;
    bridge.boot_sync_pending=pending;bridge.boot_prefix_discarded=discarded;
    for(unsigned i=0;i<keep;i++)bridge.output[(bridge.output_head+i)%BRIDGE_USB_RING]=p[skip+i];
    bridge.output_head+=keep;bridge.rx_sequence=id;return true;
  }
  if(type==PC_ERROR) { session_lost("A reported offline/protocol/backpressure error");return true; }
  if(id!=bridge.control_id || !bridge.waiting) return true;
  if(type==PC_STATUS && n==9 && (bridge.waiting==WAIT_QUERY || bridge.waiting==WAIT_LINE)) {
    uint8_t pending=bridge.waiting;bridge.waiting=0;
    if(p[0]) {
      bridge_log("LINE FAIL code=%u; retrying while DATA is blocked\n",p[0]);
      bridge.applied_line=0;bridge.retry_at=bridge.now+500;return true;
    }
    uint32_t applied=pc_get32(p+1),driver=pc_get32(p+5);
    if(!pc_baud_supported(applied) || !driver || (pending==WAIT_LINE && applied!=bridge.request_line)) {
      session_lost("mismatched applied line coding");return true;
    }
    if(pending==WAIT_QUERY) {
      bridge.synced=true;bridge.fault=false;bridge.applied_modem=255;
      bridge.tx_sequence=bridge.tx_ack=bridge.rx_sequence=0;
      bridge.last_pong=bridge.now;
    }
    bridge.applied_line=applied;bridge.driver=driver;
    bridge_log("LINE OK baud=%lu parity=%s driver=%lu\n",(unsigned long)(applied&PC_LINE_BAUD_MASK),
       (applied&PC_LINE_EVEN) ? "E" : "N",(unsigned long)driver);
    return true;
  }
  if(type==PC_MODEM_ACK && n==2 && bridge.waiting==WAIT_MODEM) {
    bridge.waiting=0;
    if(p[0] || p[1]!=bridge.request_modem) {
      bridge.applied_modem=255;bridge.retry_at=bridge.now+500;
      bridge_log("MODEM FAIL code=%u; DATA blocked\n",p[0]);return true;
    }
    bridge.applied_modem=p[1];
    bridge_log("MODEM OK dtr=%u rts=%u\n",p[1]&1U,(p[1]>>1)&1U);return true;
  }
  return true;
}
static void send_control(uint8_t kind,uint8_t wait,const uint8_t *p,unsigned n)
{
  uint16_t id=next_control();
  if(bridge_port_send(kind,id,p,n)) {
    bridge.waiting=wait;bridge.sent_at=bridge.now;
    if(wait==WAIT_LINE)bridge.request_line=bridge.desired_line;
    if(wait==WAIT_MODEM)bridge.request_modem=bridge.desired_modem;
  }
}
void bridge_step(uint32_t now)
{
  bridge.now=now;
  bridge_auto_boot_step();
  bridge_settle_lines();
  unsigned available=bridge.output_head-bridge.output_tail;
  if(available && bridge.mounted) {
    unsigned n=BRIDGE_USB_RING-bridge.output_tail%BRIDGE_USB_RING;if(n>available)n=available;
    unsigned wrote=bridge_usb_write(bridge.output+bridge.output_tail%BRIDGE_USB_RING,n);
    bridge.output_tail+=wrote;bridge.usb_tx_bytes+=wrote;
  }
  if(now-bridge.last_ping>=1000U) {
    if(bridge_port_send(PC_PING,0,NULL,0)) bridge.last_ping=now;
  }
  if(bridge.synced && now-bridge.last_pong>=4000U) session_lost("A heartbeat timeout");
  if(bridge.waiting) {
    if(now-bridge.sent_at>=6000U) session_lost("configuration ACK timeout");
    return;
  }
  if((int32_t)(now-bridge.retry_at)<0) return;
  if(!bridge.synced) {
    if(bridge.mounted) send_control(PC_QUERY,WAIT_QUERY,NULL,0);
    return;
  }
  if(!bridge.mounted) {
    if(bridge.applied_modem!=0) { uint8_t bits=0;send_control(PC_MODEM,WAIT_MODEM,&bits,1); }
    return;
  }
  /* Allow old blocks to finish before changing coding. Never consume new USB
   * DATA until the exact requested setting and modem state are acknowledged. */
  if(bridge.tx_sequence==bridge.tx_ack && !bridge.pending_length) {
    bool modem_dirty=bridge.applied_modem!=bridge.desired_modem;
    if(modem_dirty && (bridge.desired_modem&1U)) {
      uint8_t bits=bridge.desired_modem;send_control(PC_MODEM,WAIT_MODEM,&bits,1);return;
    }
    if(bridge.valid_line && bridge.applied_line!=bridge.desired_line) {
      uint8_t line[4];pc_put32(line,bridge.desired_line);send_control(PC_SET_BAUD,WAIT_LINE,line,4);return;
    }
    if(modem_dirty) { uint8_t bits=bridge.desired_modem;send_control(PC_MODEM,WAIT_MODEM,&bits,1);return; }
  }
  bool can_read=bridge_ready() && !(bridge.desired_modem&1U);
  /* Already accepted bytes belong to the old coding. Drain that partial
   * batch before applying a new line/reset request, without reading new USB
   * bytes. Otherwise a setting change with a partial batch would deadlock. */
  bool drain_old=bridge.pending_length && bridge.synced && !bridge.fault
      && bridge.applied_line && !(bridge.applied_modem&1U);
  if(!can_read && !drain_old) return;
  if((uint16_t)(bridge.tx_sequence-bridge.tx_ack)>=PC_DATA_WINDOW) return;
  if(can_read && bridge.pending_length<PC_MAX_DATA) {
    unsigned count=bridge_usb_read(bridge.pending+bridge.pending_length,PC_MAX_DATA-bridge.pending_length);
    if(count) { bridge.pending_length+=count;bridge.last_usb_rx=now; }
  }
  bool full=bridge.pending_length==PC_MAX_DATA;
  bool idle=(uint32_t)(now-bridge.last_usb_rx)>=BRIDGE_USB_IDLE_MS;
  if(bridge.pending_length && (full || idle || !can_read)) {
    uint16_t sequence=(uint16_t)(bridge.tx_sequence+1U);
    if(bridge_port_send(PC_DATA,sequence,bridge.pending,bridge.pending_length)) {
      bridge.tx_batches++;
      if(full) bridge.tx_full_batches++;
      else if(!can_read) bridge.tx_config_flush_batches++;
      else bridge.tx_idle_batches++;
      bridge.usb_rx_bytes+=bridge.pending_length;bridge.pending_length=0;bridge.tx_sequence=sequence;
    }
  }
}
