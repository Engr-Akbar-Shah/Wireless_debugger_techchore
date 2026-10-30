#ifndef PC_LINK_H
#define PC_LINK_H
#include <stdint.h>
#include <string.h>
/* CRC-protected, zero-delimited COBS packets. Bounded size for every payload. */
#define PC_DATA 1U
#define PC_SET_BAUD 2U
#define PC_QUERY 3U
#define PC_STATUS 4U
#define PC_ERROR 5U
#define PC_MODEM 6U
#define PC_MODEM_ACK 7U
#define PC_DATA_ACK 8U
#define PC_PING 9U
#define PC_PONG 10U
#define PC_DATA_WINDOW 24U
#define PC_LINE_EVEN 0x80000000U
#define PC_LINE_BAUD_MASK 0x3fffffffU
#define PC_LINK_VERSION 3U
#define PC_MODEM_OPERATION 16U
#define PC_MAX_DATA 256U
#define PC_RAW_MAX (6U + PC_MAX_DATA + 4U)
#define PC_WIRE_MAX (PC_RAW_MAX + PC_RAW_MAX / 254U + 2U)
enum { BAUD_OK=0, BAUD_UNSUPPORTED=1, BAUD_BUSY=2, BAUD_UART_ERROR=3,
       BAUD_OFFLINE=4, BAUD_TIMEOUT=5, BAUD_PROTOCOL=6 };
static inline uint32_t pc_get32(const uint8_t *p)
{ return (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24); }
static inline void pc_put32(uint8_t *p, uint32_t v)
{ for (unsigned i=0;i<4;i++) p[i]=(uint8_t)(v>>(8U*i)); }
static inline int pc_baud_supported(uint32_t baud)
{ baud &= PC_LINE_BAUD_MASK;return baud==9600U || baud==57600U || baud==115200U || baud==230400U || baud==460800U || baud==921600U; }
static inline uint32_t pc_crc32(const uint8_t *p, unsigned n)
{
  uint32_t crc=0xffffffffU;
  while(n--) { crc^=*p++; for(unsigned i=0;i<8;i++) crc=(crc>>1)^((0U-(crc&1U))&0xedb88320U); }
  return ~crc;
}
static inline int pc_shape(uint8_t type, unsigned n)
{ return (type==PC_DATA && n>0 && n<=PC_MAX_DATA) || (type==PC_SET_BAUD && n==4)
      || (type==PC_MODEM && n==1) || (type==PC_MODEM_ACK && n==2)
      || ((type==PC_PING || type==PC_PONG || type==PC_DATA_ACK) && n==0) || (type==PC_QUERY && n==0) || (type==PC_STATUS && n==9) || (type==PC_ERROR && n==1); }
/* Standard COBS. Capacity is n+n/254+1, plus the zero delimiter. */
static inline unsigned pc_cobs_encode(uint8_t *out,const uint8_t *raw,unsigned n)
{
  unsigned code_at=0,write=1;uint8_t code=1;
  for(unsigned i=0;i<n;i++) {
    if(raw[i]==0) {out[code_at]=code;code_at=write++;code=1;}
    else {
      out[write++]=raw[i];
      if(++code==0xffU) {out[code_at]=code;code_at=write++;code=1;}
    }
  }
  out[code_at]=code;return write;
}
static inline int pc_cobs_decode(uint8_t *raw,unsigned capacity,const uint8_t *wire,unsigned n)
{
  unsigned read=0,write=0;
  while(read<n) {
    unsigned code=wire[read++];
    if(!code || code-1U>n-read || code-1U>capacity-write)return -1;
    for(unsigned i=1;i<code;i++)raw[write++]=wire[read++];
    if(code!=0xffU && read<n) {if(write==capacity)return -1;raw[write++]=0;}
  }
  return (int)write;
}
static inline unsigned pc_encode(uint8_t *wire,uint8_t type,uint16_t id,const uint8_t *payload,unsigned n)
{
  uint8_t raw[PC_RAW_MAX];
  if(!pc_shape(type,n) || (n && !payload))return 0;
  raw[0]=PC_LINK_VERSION;raw[1]=type;raw[2]=(uint8_t)id;raw[3]=(uint8_t)(id>>8);
  raw[4]=(uint8_t)n;raw[5]=(uint8_t)(n>>8);
  if(n)memcpy(raw+6,payload,n);
  pc_put32(raw+6+n,pc_crc32(raw,6+n));
  unsigned size=pc_cobs_encode(wire,raw,n+10U);wire[size++]=0;return size;
}
typedef struct {
  uint8_t raw[PC_RAW_MAX],wire[PC_WIRE_MAX-1U];
  unsigned used,length;uint16_t id;uint8_t type,discard;
} pc_parser_t;
/*1 valid,-1 malformed,0 incomplete. A delimiter always re-arms the parser. */
static inline int pc_feed(pc_parser_t *p,uint8_t c)
{
  if(c) {
    if(p->used>=sizeof(p->wire))p->discard=1;
    else if(!p->discard)p->wire[p->used++]=c;
    return 0;
  }
  if(!p->used && !p->discard)return 0;
  int decoded=p->discard ? -1 : pc_cobs_decode(p->raw,sizeof(p->raw),p->wire,p->used);
  p->used=0;p->discard=0;
  if(decoded<10 || p->raw[0]!=PC_LINK_VERSION)return -1;
  unsigned n=p->raw[4]|((unsigned)p->raw[5]<<8);
  if(n>PC_MAX_DATA || (unsigned)decoded!=n+10U || !pc_shape(p->raw[1],n)
      || pc_get32(p->raw+6+n)!=pc_crc32(p->raw,6+n))return -1;
  p->length=n;p->type=p->raw[1];p->id=p->raw[2]|((uint16_t)p->raw[3]<<8);return 1;
}
#endif
