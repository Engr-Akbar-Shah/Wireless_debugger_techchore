#ifndef PC_HOST_H
#define PC_HOST_H
static osMutexId_t pc_output_mutex;
static osMutexId_t pc_return_mutex;
#define PC_RETURN_QUEUE_SIZE 8192U
/* PC/USB backpressure is a delay, not a failure: radio credits already stop the
 * sender. Only a much longer silence (below A's 15s watchdog) ends the session. */
#define PC_BACKPRESSURE_LIMIT_S 10U
_Static_assert(PC_DATA_WINDOW*(PC_WIRE_MAX+12U)+512U<UART_RING_SIZE,"Wire window and controls must fit A RX ring");
static uint8_t pc_return_data[PC_RETURN_QUEUE_SIZE];
static uint32_t pc_return_head,pc_return_tail;
static uint32_t pc_return_wait_since;
static uint32_t pc_return_wait_epoch;
static bool pc_return_waiting;
volatile uint32_t pc_return_admitted_bytes,pc_return_sent_bytes,pc_return_discarded_bytes;
volatile uint32_t pc_return_queue_peak,pc_return_enqueue_wait_max_ticks,pc_return_enqueue_timeouts;
static pc_parser_t pc_input;
static bool pc_request_pending,pc_modem_pending;
static volatile bool pc_baud_known;
static volatile uint32_t pc_session_generation;
static uint16_t pc_request_id,pc_modem_id;
static uint32_t pc_requested_baud;
static uint8_t pc_modem_bits,pc_modem_applied;
static uint32_t pc_last_activity;
static uint16_t pc_data_rx_sequence;
static volatile uint16_t pc_data_tx_sequence;
static volatile uint16_t pc_data_tx_ack;
static uint8_t pc_last_data[PC_MAX_DATA];
static unsigned pc_last_data_length;
volatile uint32_t pc_bad_frames;
__attribute__((section(".noinit"))) volatile uint32_t pc_bad_debug[4];
__attribute__((section(".noinit"))) uint8_t pc_bad_raw[PC_RAW_MAX];
static struct { uint8_t data[PC_MAX_DATA];uint16_t length,id;uint32_t tick; } pc_pending_data[PC_DATA_WINDOW];
static uint32_t pc_pending_head,pc_pending_tail;
static uint16_t pc_forwarded_sequence;
volatile uint32_t pc_mutex_max_ticks,pc_uart_enqueue_max_ticks,pc_ack_wait_max_ticks,pc_ack_wait_total_ticks,pc_ack_wait_events,pc_input_ack_max_ticks,pc_service_gap_max_ticks,pc_service_last_tick;
__attribute__((section(".noinit"))) volatile uint32_t pc_return_failure[12];
static void __attribute__((unused)) pc_save_return_stats(void)
{
  pc_return_failure[0]=0;
  pc_return_failure[1]=pc_return_queue_peak;pc_return_failure[2]=pc_return_enqueue_wait_max_ticks;
  pc_return_failure[3]=pc_ack_wait_max_ticks;pc_return_failure[4]=pc_ack_wait_total_ticks;
  pc_return_failure[5]=pc_input_ack_max_ticks;pc_return_failure[6]=pc_service_gap_max_ticks;
  pc_return_failure[7]=pc_return_admitted_bytes;pc_return_failure[8]=pc_return_sent_bytes;
  pc_return_failure[9]=pc_data_tx_sequence;pc_return_failure[10]=pc_data_tx_ack;
  pc_return_failure[11]=pc_uart_enqueue_max_ticks;__DMB();pc_return_failure[0]=0x50524631U;
}
static bool pc_send_packet_epoch(uint8_t type,uint16_t id,const uint8_t *data,unsigned n,uint32_t epoch)
{
  uint8_t wire[PC_WIRE_MAX];
  uint32_t started=osKernelGetTickCount();
  if(!pc_output_mutex || osMutexAcquire(pc_output_mutex,osKernelGetTickFreq())!=osOK)return false;
  uint32_t waited=osKernelGetTickCount()-started;if(waited>pc_mutex_max_ticks)pc_mutex_max_ticks=waited;
  if(type==PC_DATA) {
    /* Serialize DATA allocation with explicit session reset. Old TCP blocks
     * cannot queue behind the new STATUS and leak into its USB session. */
    if(epoch!=pc_session_generation || !pc_baud_known) {osMutexRelease(pc_output_mutex);return true;}
    id=++pc_data_tx_sequence;
  }
  unsigned length=pc_encode(wire,type,id,data,n);
  started=osKernelGetTickCount();
  bool ok=length && host_uart_write(wire,length);
  waited=osKernelGetTickCount()-started;if(waited>pc_uart_enqueue_max_ticks)pc_uart_enqueue_max_ticks=waited;
  osMutexRelease(pc_output_mutex);return ok;
}
static bool pc_send_packet(uint8_t type,uint16_t id,const uint8_t *data,unsigned n)
{ return pc_send_packet_epoch(type,id,data,n,pc_session_generation); }
/* Called with pc_return_mutex held. Reset only unsent payload; bytes already
 * framed retain their sequence until the explicit QUERY session reset. */
static void pc_return_discard(void)
{
  pc_return_discarded_bytes+=pc_return_head-pc_return_tail;
  pc_return_tail=pc_return_head;
}
static bool pc_return_idle(void)
{
  if(osMutexAcquire(pc_return_mutex,osKernelGetTickFreq())!=osOK)return false;
  bool idle=pc_return_head==pc_return_tail && pc_data_tx_sequence==pc_data_tx_ack;
  osMutexRelease(pc_return_mutex);return idle;
}
#define PC_RADIO_IDLE_MS 6U
static uint16_t pc_pending_offset;
static uint32_t pc_last_data_tick;
static uint8_t pc_radio_batch[BRIDGE_DATA_CHUNK];
volatile uint32_t pc_radio_batches,pc_radio_full_batches,pc_radio_idle_batches;
static unsigned pc_radio_pending_length(void)
{
  unsigned n=0;uint32_t tail=pc_pending_tail;unsigned offset=pc_pending_offset;
  while(tail!=pc_pending_head && n<BRIDGE_DATA_CHUNK) {
    unsigned take=pc_pending_data[tail%PC_DATA_WINDOW].length-offset;
    if(take>BRIDGE_DATA_CHUNK-n)take=BRIDGE_DATA_CHUNK-n;
    n+=take;tail++;offset=0;
  }
  return n;
}
static unsigned pc_radio_ready_length(void)
{
  unsigned n=pc_radio_pending_length();
  if(n<BRIDGE_DATA_CHUNK && pc_pending_head-pc_pending_tail<PC_DATA_WINDOW
      && osKernelGetTickCount()-pc_last_data_tick<PC_RADIO_IDLE_MS)return 0;
  return n;
}
/* Payload awaiting its short batch deadline also defers credits. Sending
 * a tiny credit before every growing DATA batch defeats coalescing. Only
 * credit-blocked payload needs an immediate standalone credit. ACKs/PINGs neither
 * count as payload nor restart the payload-only short-batch deadline. */
static bool pc_payload_can_piggyback_credit(void)
{
  unsigned n=pc_radio_pending_length();
  return n && bridge_credit_can_send(network_tx_total,network_peer_delivered,n);
}
static bool pc_forward_pending(int socket_id)
{
  unsigned n=pc_radio_ready_length();
  if(!n || !bridge_credit_can_send(network_tx_total,network_peer_delivered,n))return true;
  uint32_t next_tail=pc_pending_tail;unsigned offset=pc_pending_offset,used=0;
  uint16_t acknowledged=pc_forwarded_sequence;
  while(used<n) {
    unsigned slot=next_tail%PC_DATA_WINDOW,take=pc_pending_data[slot].length-offset;
    if(take>n-used)take=n-used;
    memcpy(pc_radio_batch+used,pc_pending_data[slot].data+offset,take);used+=take;offset+=take;
    if(offset==pc_pending_data[slot].length) {
      uint32_t wait=osKernelGetTickCount()-pc_pending_data[slot].tick;
      if(wait>pc_input_ack_max_ticks)pc_input_ack_max_ticks=wait;
      acknowledged=pc_pending_data[slot].id;next_tail++;offset=0;
    }
  }
  if(network_send_frame(socket_id,BRIDGE_DATA,pc_radio_batch,(uint16_t)n,0U)<0)return false;
  for(unsigned i=0;i<n;i++)bridge_tcp_tx_hash=(bridge_tcp_tx_hash^pc_radio_batch[i])*16777619U;
  bridge_tcp_sent_bytes+=n;pc_radio_batches++;
  if(n==BRIDGE_DATA_CHUNK)pc_radio_full_batches++;else pc_radio_idle_batches++;
  pc_pending_tail=next_tail;pc_pending_offset=(uint16_t)offset;
  if(acknowledged!=pc_forwarded_sequence) {
    pc_forwarded_sequence=acknowledged;
    if(!pc_send_packet(PC_DATA_ACK,acknowledged,NULL,0U))return false;
  }
  return true;
}
static bool pc_send_status(uint16_t id,uint8_t status,uint32_t applied,uint32_t driver)
{
  uint8_t response[9];response[0]=status;pc_put32(response+1,applied);pc_put32(response+5,driver);
  return pc_send_packet(PC_STATUS,id,response,sizeof(response));
}
static bool payload_uart_write(const uint8_t *data,uint32_t length)
{
  uint32_t epoch=pc_session_generation;
  uint32_t started=osKernelGetTickCount();
  if(!length)return true;
  if(length>BRIDGE_FRAME_MAX)return false;
  if(!pc_baud_known)return true; /* No acknowledged host session: discard outage bytes. */
  while(1) {
    if(osMutexAcquire(pc_return_mutex,osKernelGetTickFreq())!=osOK)return false;
    if(epoch!=pc_session_generation || !pc_baud_known) {
      osMutexRelease(pc_return_mutex);return true;
    }
    if(network_peer_closed) {osMutexRelease(pc_return_mutex);return false;}
    uint32_t used=pc_return_head-pc_return_tail;
    if(length<=PC_RETURN_QUEUE_SIZE-used) {
      uint32_t index=pc_return_head%PC_RETURN_QUEUE_SIZE;
      uint32_t first=PC_RETURN_QUEUE_SIZE-index;if(first>length)first=length;
      memcpy(pc_return_data+index,data,first);
      memcpy(pc_return_data,data+first,length-first);
      __DMB();pc_return_head+=length;pc_return_admitted_bytes+=length;
      used+=length;if(used>pc_return_queue_peak)pc_return_queue_peak=used;
      bridge_trace_event(TRACE_RETURN_ADMITTED,pc_return_admitted_bytes,used,false);
      osMutexRelease(pc_return_mutex);
      uint32_t waited=osKernelGetTickCount()-started;
      if(waited>pc_return_enqueue_wait_max_ticks)pc_return_enqueue_wait_max_ticks=waited;
      return true; /* Radio credit follows complete admission, never Pico ACK. */
    }
    osMutexRelease(pc_return_mutex);
    if(osKernelGetTickCount()-started>=osKernelGetTickFreq()/4U)
      bridge_trace_event(TRACE_RETURN_STALL,pc_return_head,pc_return_tail,true);
    if(osKernelGetTickCount()-started>=PC_BACKPRESSURE_LIMIT_S*osKernelGetTickFreq()) {
      pc_return_enqueue_timeouts++;return false;
    }
    osDelay(1U);
  }
}
/* One worker owns the queue tail. Never wait for an ACK with either mutex
 * held: radio admission and PC control/ACK parsing must continue independently. */
static bool pc_return_service(void)
{
  uint8_t packet[PC_MAX_DATA];
  if(osMutexAcquire(pc_return_mutex,osKernelGetTickFreq())!=osOK)return false;
  if(!pc_baud_known || network_peer_closed)pc_return_discard();
  uint32_t used=pc_return_head-pc_return_tail,epoch=pc_session_generation;
  if(pc_return_wait_epoch!=epoch) {pc_return_waiting=false;pc_return_wait_epoch=epoch;}
  if(!used) {osMutexRelease(pc_return_mutex);pc_return_waiting=false;return true;}
  if((uint16_t)(pc_data_tx_sequence-pc_data_tx_ack)>=PC_DATA_WINDOW) {
    osMutexRelease(pc_return_mutex);
    if(!pc_return_waiting) {pc_return_waiting=true;pc_return_wait_since=osKernelGetTickCount();pc_ack_wait_events++;}
    uint32_t waited=osKernelGetTickCount()-pc_return_wait_since;
    if(waited>=osKernelGetTickFreq()/4U)
      bridge_trace_event(TRACE_RETURN_STALL,pc_data_tx_sequence,pc_data_tx_ack,true);
    if(waited>=PC_BACKPRESSURE_LIMIT_S*osKernelGetTickFreq()) {
      pc_ack_wait_total_ticks+=waited;if(waited>pc_ack_wait_max_ticks)pc_ack_wait_max_ticks=waited;
      pc_return_waiting=false;return false;
    }
    return true;
  }
  if(pc_return_waiting) {
    uint32_t waited=osKernelGetTickCount()-pc_return_wait_since;
    pc_ack_wait_total_ticks+=waited;if(waited>pc_ack_wait_max_ticks)pc_ack_wait_max_ticks=waited;
    pc_return_waiting=false;
  }
  uint32_t n=used>PC_MAX_DATA ? PC_MAX_DATA : used;
  uint32_t tail=pc_return_tail,index=tail%PC_RETURN_QUEUE_SIZE;
  uint32_t first=PC_RETURN_QUEUE_SIZE-index;if(first>n)first=n;
  memcpy(packet,pc_return_data+index,first);memcpy(packet+first,pc_return_data,n-first);
  osMutexRelease(pc_return_mutex);
  if(!pc_send_packet_epoch(PC_DATA,0,packet,n,epoch))return false;
  if(osMutexAcquire(pc_return_mutex,osKernelGetTickFreq())!=osOK)return false;
  if(epoch==pc_session_generation && pc_baud_known && !network_peer_closed) {
    pc_return_tail=tail+n;pc_return_sent_bytes+=n;
    bridge_trace_event(TRACE_RETURN_SENT,pc_return_sent_bytes,pc_return_head-pc_return_tail,false);
  }
  osMutexRelease(pc_return_mutex);return true;
}
static void __attribute__((unused)) pc_output_worker(void *argument)
{
  (void)argument;
  while(1) {
    uint32_t before=pc_return_tail;
    if(!pc_return_service())network_stop(19U,pc_return_head-pc_return_tail);
    /* Drain ready work, like the original UART worker. The bounded
     * acknowledgement window bounds each burst. Sleep only when empty or
     * blocked; a tick per256bytes unnecessarily couples throughput to the
     * scheduling of the other two bridge tasks. */
    if(pc_return_tail==before || network_peer_closed)osDelay(1U);
    else osThreadYield(); /* Give a mutex waiter/ACK parser its turn without a timed pause. */
  }
}
static bool pc_host_service(int socket_id)
{
  uint32_t tick=osKernelGetTickCount();
  if(pc_service_last_tick && tick-pc_service_last_tick>pc_service_gap_max_ticks)pc_service_gap_max_ticks=tick-pc_service_last_tick;
  pc_service_last_tick=tick;
  bool offline=socket_id<0 || network_peer_closed;
  if(pc_request_pending && (offline || network_baud_status>=2U)) {
    uint8_t status=offline ? BAUD_OFFLINE : network_baud_result;
    uint32_t applied=offline ? 0U : network_baud_applied;
    uint32_t driver=offline ? 0U : network_baud_driver;
    if(status==BAUD_OK && pc_requested_baud && applied!=pc_requested_baud) status=BAUD_PROTOCOL;
    if(!pc_send_status(pc_request_id,status,applied,driver)) return false;
    pc_request_pending=false;pc_baud_known=status==BAUD_OK;
  }
  if(pc_modem_pending && (offline || bridge_control_status>=2U)) {
    uint8_t response[2]={offline ? BAUD_OFFLINE : (bridge_control_status==2U ? BAUD_OK : BAUD_UART_ERROR),pc_modem_bits};
    if(!pc_send_packet(PC_MODEM_ACK,pc_modem_id,response,2U)) return false;
    if(response[0]==BAUD_OK) pc_modem_applied=pc_modem_bits;
    pc_modem_pending=false;
  }
  if(!offline && pc_modem_applied && !pc_modem_pending && !pc_request_pending
      && network_elapsed(pc_last_activity,3000U)) {
    bridge_control_request=BRIDGE_RELEASE;pc_modem_applied=0;pc_baud_known=false;
  }
  unsigned budget=1024U;
  while(budget-- && ring_tail!=ring_head) {
    uint32_t tail=ring_tail;uint8_t byte=uart_ring[tail];ring_tail=(tail+1U)%UART_RING_SIZE;
    unsigned prior_used=pc_input.used;
    int complete=pc_feed(&pc_input,byte);
    if(complete<0) {
      pc_bad_frames++;
      pc_bad_debug[0]=0x50434244U;pc_bad_debug[1]=prior_used;
      pc_bad_debug[2]=pc_data_rx_sequence;pc_bad_debug[3]=osKernelGetTickCount();
      memcpy(pc_bad_raw,pc_input.raw,PC_RAW_MAX);continue;
    }
    if(!complete) continue;
    pc_last_activity=osKernelGetTickCount();
    if(pc_input.type==PC_PING) {
      uint8_t status=BAUD_OFFLINE;
      if(!offline) { if(!pc_send_packet(PC_PONG,pc_input.id,NULL,0)) return false; }
      else if(!pc_send_packet(PC_ERROR,pc_input.id,&status,1)) return false;
    } else if(pc_input.type==PC_DATA_ACK) {
      uint16_t advance=(uint16_t)(pc_input.id-pc_data_tx_ack);
      uint16_t sent=pc_data_tx_sequence;
      if(advance>(uint16_t)(sent-pc_data_tx_ack)) {
        if((uint16_t)(pc_data_tx_ack-pc_input.id)<=PC_DATA_WINDOW) continue;
        network_stop(16U,((uint32_t)sent<<16)|pc_input.id);return false;
      }
      pc_data_tx_ack=pc_input.id;
    } else if(pc_input.type==PC_SET_BAUD || pc_input.type==PC_QUERY) {
      uint32_t requested=pc_input.type==PC_QUERY ? 0U : pc_get32(pc_input.raw+6);
      if(pc_request_pending || pc_modem_pending || (pc_input.type==PC_SET_BAUD
          && (pc_pending_head!=pc_pending_tail || !pc_return_idle()))) {
        if(!pc_send_status(pc_input.id,BAUD_BUSY,0,0)) return false;
      } else if(offline) {
        pc_baud_known=false;
        if(!pc_send_status(pc_input.id,BAUD_OFFLINE,0,0)) return false;
      } else if(pc_input.type==PC_SET_BAUD && (!pc_baud_supported(requested) || (requested&0x40000000U))) {
        if(!pc_send_status(pc_input.id,BAUD_UNSUPPORTED,network_baud_applied,network_baud_driver)) return false;
      } else {
        if(pc_input.type==PC_QUERY) {
          /* QUERY explicitly starts a new host session. Queued old DATA is
           * outside its integrity interval, not a reason to wait forever. */
          if(osMutexAcquire(pc_output_mutex,osKernelGetTickFreq())!=osOK)return false;
          if(osMutexAcquire(pc_return_mutex,osKernelGetTickFreq())!=osOK) {osMutexRelease(pc_output_mutex);return false;}
          pc_baud_known=false;pc_session_generation++;
          pc_return_discard();
          pc_data_tx_sequence=pc_data_tx_ack=pc_data_rx_sequence=pc_forwarded_sequence=0U;
          pc_pending_tail=pc_pending_head;pc_pending_offset=0;pc_last_data_length=0;
          osMutexRelease(pc_return_mutex);
          osMutexRelease(pc_output_mutex);
        }
        pc_requested_baud=requested;pc_request_id=pc_input.id;
        pc_request_pending=true;pc_baud_known=false;
        network_baud_request=requested;__DMB();network_baud_status=1U;
      }
    } else if(pc_input.type==PC_MODEM) {
      uint8_t response[2]={BAUD_OK,pc_input.raw[6]};
      if(response[1]>3U) response[0]=BAUD_UNSUPPORTED;
      else if(offline) response[0]=BAUD_OFFLINE;
      else if(pc_request_pending || pc_modem_pending) response[0]=BAUD_BUSY;
      else {
        /* Reset/BOOT changes intentionally abandon unsent old-target bytes.
         * Serialize against a worker that has copied, but not framed, a block. */
        if(osMutexAcquire(pc_output_mutex,osKernelGetTickFreq())!=osOK)return false;
        if(osMutexAcquire(pc_return_mutex,osKernelGetTickFreq())!=osOK) {osMutexRelease(pc_output_mutex);return false;}
        pc_session_generation++;pc_return_discard();
        osMutexRelease(pc_return_mutex);osMutexRelease(pc_output_mutex);
        pc_modem_bits=response[1];pc_modem_id=pc_input.id;pc_modem_pending=true;
        bridge_control_status=0;__DMB();bridge_control_request=PC_MODEM_OPERATION+pc_modem_bits;
        continue;
      }
      if(!pc_send_packet(PC_MODEM_ACK,pc_input.id,response,2U)) return false;
    } else if(pc_input.type==PC_DATA) {
      uint8_t status=BAUD_BUSY;
      if(offline) status=BAUD_OFFLINE;
      else if(pc_baud_known && !pc_request_pending && !pc_modem_pending) status=BAUD_OK;
      if(status!=BAUD_OK) {
        if(!pc_send_packet(PC_ERROR,pc_input.id,&status,1U)) return false;
        continue;
      }
      uint16_t n=(uint16_t)pc_input.length;
      if(pc_input.id==pc_data_rx_sequence && pc_last_data_length==n
         && !memcmp(pc_last_data,pc_input.raw+6,n)) {
        if(pc_forwarded_sequence==pc_input.id && !pc_send_packet(PC_DATA_ACK,pc_input.id,NULL,0U)) return false;
        continue;
      }
      if(pc_input.id!=(uint16_t)(pc_data_rx_sequence+1U)) {
        network_stop(17U,((uint32_t)(uint16_t)(pc_data_rx_sequence+1U)<<16)|pc_input.id);return false;
      }
      if(pc_pending_head-pc_pending_tail>=PC_DATA_WINDOW) { network_stop(18U,pc_pending_head-pc_pending_tail);return false; }
      unsigned slot=pc_pending_head%PC_DATA_WINDOW;
      memcpy(pc_pending_data[slot].data,pc_input.raw+6,n);
      pc_last_data_tick=osKernelGetTickCount();pc_pending_data[slot].tick=pc_last_data_tick;pc_pending_data[slot].length=n;pc_pending_data[slot].id=pc_input.id;pc_pending_head++;
      pc_data_rx_sequence=pc_input.id;memcpy(pc_last_data,pc_input.raw+6,n);pc_last_data_length=n;
    } else {
      uint8_t status=BAUD_PROTOCOL;if(!pc_send_packet(PC_ERROR,pc_input.id,&status,1U)) return false;
    }
  }
  return offline ? true : pc_forward_pending(socket_id);
}
#endif
