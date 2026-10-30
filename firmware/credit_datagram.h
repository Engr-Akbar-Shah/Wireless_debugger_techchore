/* Only delivery counters use UDP. Payload remains ordered/reliable TCP. */
#ifndef CREDIT_DATAGRAM_H
#define CREDIT_DATAGRAM_H
#include <stdbool.h>
#include "bridge_protocol.h"
#define BRIDGE_CREDIT_PORT 5006U
#define BRIDGE_CREDIT_DATAGRAM_SIZE 12U
static inline void credit_put32(uint8_t *p,uint32_t v)
{p[0]=v;p[1]=v>>8;p[2]=v>>16;p[3]=v>>24;}
static inline uint32_t credit_get32(const uint8_t *p)
{return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static inline void credit_datagram_encode(uint8_t *p,uint32_t nonce,uint32_t delivered)
{p[0]='W';p[1]='C';p[2]=BRIDGE_PROTOCOL_VERSION;p[3]=0;credit_put32(p+4,nonce);credit_put32(p+8,delivered);}
static inline bool credit_datagram_decode(const uint8_t *p,uint32_t length,uint32_t nonce,
    uint32_t sent,uint32_t previous,uint32_t *delivered)
{
 if(length!=BRIDGE_CREDIT_DATAGRAM_SIZE || p[0]!='W' || p[1]!='C'
    || p[2]!=BRIDGE_PROTOCOL_VERSION || p[3]!=0 || credit_get32(p+4)!=nonce)return false;
 uint32_t value=credit_get32(p+8);
 if(!bridge_credit_valid(sent,previous,value))return false;
 *delivered=value;return true;
}
#endif
