#include "bridge_engine.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t kind,sent[PC_MAX_DATA],usb_in[4096],usb_out[16384];
static uint16_t request_id;static unsigned sent_n,usb_length,usb_read_at,usb_out_n,usb_limit=4096;
static bool allow_send=true;
bool bridge_port_send(uint8_t k,uint16_t id,const uint8_t *p,unsigned n)
{ if(!allow_send)return false;kind=k;request_id=id;sent_n=n;if(n)memcpy(sent,p,n);return true; }
unsigned bridge_usb_read(uint8_t *p,unsigned n)
{ unsigned left=usb_length-usb_read_at;if(n>left)n=left;memcpy(p,usb_in+usb_read_at,n);usb_read_at+=n;return n; }
unsigned bridge_usb_write(const uint8_t *p,unsigned n)
{ if(n>usb_limit)n=usb_limit;memcpy(usb_out+usb_out_n,p,n);usb_out_n+=n;return n; }
void bridge_usb_discard_output(void){}
void bridge_usb_flush(void){usb_length=usb_read_at=0;}
void bridge_log(const char *format,...){(void)format;}
static void line_ack(uint32_t line)
{uint8_t p[9]={0};pc_put32(p+1,line);pc_put32(p+5,(line&PC_LINE_EVEN) ? line&PC_LINE_BAUD_MASK : (uint32_t)((uint64_t)line*10075/10000));assert(bridge_packet(PC_STATUS,request_id,p,9));}
static void modem_ack(void)
{uint8_t p[2]={0,sent[0]};assert(bridge_packet(PC_MODEM_ACK,request_id,p,2));}
int main(void)
{
 bridge_init();bridge_mounted(true);bridge_step(0);assert(kind==PC_QUERY && !bridge_ready());
 line_ack(921600);bridge_step(1);assert(kind==PC_SET_BAUD && pc_get32(sent)==115200);
 line_ack(115200);bridge_step(2);assert(kind==PC_MODEM && sent[0]==0);modem_ack();assert(bridge_ready());
 for(unsigned i=0;i<256;i++)usb_in[i]=(uint8_t)i;
 usb_length=256;
 bridge_step(3);assert(kind==PC_DATA && sent_n==256 && !memcmp(sent,usb_in,256));
 bridge_line_changed(460800,8,0,0);bridge_step(4);assert(bridge.waiting==0); /* Old DATA must drain. */
 assert(bridge_packet(PC_DATA_ACK,1,NULL,0));bridge_step(5);assert(kind==PC_SET_BAUD && pc_get32(sent)==460800);
 line_ack(460800);assert(bridge_ready());
 uint8_t binary[256];for(unsigned i=0;i<256;i++)binary[i]=(uint8_t)i;
 assert(bridge_packet(PC_DATA,1,binary,256));usb_limit=13;
 for(unsigned i=6;i<26;i++)bridge_step(i);
 assert(usb_out_n==256 && !memcmp(usb_out,binary,256));
 bridge_modem_changed(true,true);bridge_step(30);assert(kind==PC_MODEM && sent[0]==3);modem_ack();
 bridge_line_changed(115200,8,2,0);bridge_modem_changed(false,true);
 bridge_step(31);assert(kind==PC_SET_BAUD && pc_get32(sent)==(115200|PC_LINE_EVEN));
 line_ack(115200|PC_LINE_EVEN);bridge_step(32);assert(kind==PC_MODEM && sent[0]==2);modem_ack();assert(bridge_ready());
 bridge.boot_sync_pending=false;bridge.auto_boot=0; /* Independent generic binary checks below; auto boot is tested in test_modem_mapping.c. */
 bridge_line_changed(115200,7,0,0);assert(!bridge_ready());bridge_line_changed(115200,8,2,0);assert(bridge_ready());
 bridge_line_changed(57600,8,0,0);bridge_step(35);assert(bridge.waiting);
 bridge_line_changed(9600,8,0,0);line_ack(57600);assert(!bridge_ready());bridge_step(36);assert(pc_get32(sent)==9600);line_ack(9600);
 allow_send=false;usb_length=512;usb_read_at=256;bridge_step(37);assert(bridge.pending_length==256 && bridge.tx_sequence==1);
 allow_send=true;bridge_step(38);assert(bridge.pending_length==0 && bridge.tx_sequence==2);
 assert(bridge_packet(PC_DATA_ACK,2,NULL,0));
 usb_limit=0;
 for(unsigned i=0;i<32;i++)assert(bridge_packet(PC_DATA,(uint16_t)(i+2),binary,256));
 assert(!bridge_packet(PC_DATA,34,binary,256)); /* Bounded USB backpressure. */
 usb_limit=256;bridge_step(40);assert(bridge_packet(PC_DATA,34,binary,256));
 bridge_transport_fault("injected peer-restart framing error");assert(bridge.valid_line && !bridge.synced);
 bridge.synced=true;bridge.fault=false;bridge.applied_modem=2;bridge.applied_line=bridge.desired_line;bridge.retry_at=0;
 bridge_mounted(false);bridge_step(41);assert(kind==PC_MODEM && sent[0]==0);modem_ack();
 bridge_mounted(true);bridge_line_changed(230400,8,0,0);bridge_step(42);assert(bridge.waiting);
 bridge_step(6043);assert(!bridge.synced && bridge.fault && bridge.session_resets>=1 && !bridge_ready());
 /* USB partial chunks coalesce until the real idle deadline. */
 usb_limit=256;usb_length=usb_read_at=0;bridge_init();bridge_mounted(true);
 bridge_step(0);line_ack(115200);bridge_step(1);modem_ack();assert(bridge_ready());
 memcpy(usb_in,"AB",2);usb_length=2;
 bridge_step(10);assert(bridge.pending_length==2 && bridge.tx_batches==0);
 usb_in[2]='C';usb_length=3;
 bridge_step(11);assert(bridge.pending_length==3 && bridge.tx_batches==0);
 bridge_step(12);assert(bridge.pending_length==3 && bridge.tx_batches==0);
 bridge_step(13);assert(kind==PC_DATA && sent_n==3 && !memcmp(sent,"ABC",3));
 assert(bridge.tx_idle_batches==1 && bridge.tx_batches==1);
 assert(bridge_packet(PC_DATA_ACK,1,NULL,0));
 usb_in[3]='D';usb_length=4;bridge_step(14);assert(bridge.pending_length==1);
 bridge_line_changed(921600,8,0,0);bridge_step(15);
 assert(kind==PC_DATA && sent_n==1 && sent[0]=='D' && bridge.tx_config_flush_batches==1);
 assert(bridge_packet(PC_DATA_ACK,2,NULL,0));bridge_step(16);
 assert(kind==PC_SET_BAUD && pc_get32(sent)==921600);line_ack(921600);assert(bridge_ready());
 /* Only explicit boot/reset state filters its bounded pre-ACK prefix.
  * A retry cannot consume it; all bytes after ACK, including zero, survive. */
 bridge.output_head=bridge.output_tail=0;bridge.rx_sequence=0;
 bridge_modem_changed(true,true);assert(bridge.boot_sync_pending);
 bridge_modem_changed(false,true);bridge_modem_changed(false,false);
 assert(bridge.boot_sync_pending); /* Port-close gap while ROM TX is idle. */
 /* A host can pulse RTS while opening an ordinary8N1 port. That must
  * never turn arbitrary binary bytes into a ROM prefix. */
 assert(bridge_packet(PC_DATA,1,binary,256));
 assert(bridge.output_head==256 && !memcmp(bridge.output,binary,256));
 assert(bridge.boot_sync_pending && bridge.boot_prefix_discarded==0);
 bridge.output_head=bridge.output_tail=0;bridge.rx_sequence=0;
 bridge.applied_line=115200|PC_LINE_EVEN;
 uint8_t boot[]={0,0,0xff,0x79,0,0x11};allow_send=false;
 assert(!bridge_packet(PC_DATA,1,boot,sizeof(boot)) && bridge.boot_prefix_discarded==0);
 allow_send=true;assert(bridge_packet(PC_DATA,1,boot,sizeof(boot)));
 assert(!bridge.boot_sync_pending && bridge.boot_prefix_discarded==3 && bridge.output_head==3);
 assert(!memcmp(bridge.output,"\x79\x00\x11",3));
 assert(bridge_packet(PC_DATA,2,binary,256));assert(bridge.output_head==259);
 bridge_modem_changed(true,true);bridge_modem_changed(false,false);
 assert(!bridge.boot_sync_pending); /* Normal terminal reset-release is unfiltered. */
 puts("PASS Pico engine: startup, exact line ACK, binary bytes, drain guard, parity/reset ordering, unsupported format, stale snapshot, send retry, bounded backpressure, unmount release, timeout gate");
 puts("PASS Pico batching: contiguous partial USB reads,2ms actual idle, old partial batch drains before baud change");
}
