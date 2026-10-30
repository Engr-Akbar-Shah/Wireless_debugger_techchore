#include "tusb.h"
#include "pico/unique_id.h"
#include <string.h>
/* TinyUSB example VID/PID for bench development only. Assign production IDs. */
static const tusb_desc_device_t device={
  .bLength=sizeof(tusb_desc_device_t),.bDescriptorType=TUSB_DESC_DEVICE,.bcdUSB=0x0200,
  .bDeviceClass=TUSB_CLASS_MISC,.bDeviceSubClass=MISC_SUBCLASS_COMMON,.bDeviceProtocol=MISC_PROTOCOL_IAD,
  .bMaxPacketSize0=CFG_TUD_ENDPOINT0_SIZE,.idVendor=0xcafe,.idProduct=0x4012,.bcdDevice=0x0390,
  .iManufacturer=1,.iProduct=2,.iSerialNumber=3,.bNumConfigurations=1
};
uint8_t const *tud_descriptor_device_cb(void) { return (const uint8_t *)&device; }
static const uint8_t configuration[]={
  TUD_CONFIG_DESCRIPTOR(1,4,0,TUD_CONFIG_DESC_LEN+2*TUD_CDC_DESC_LEN,0,100),
  TUD_CDC_DESCRIPTOR(0,4,0x81,8,0x02,0x82,64),
  TUD_CDC_DESCRIPTOR(2,5,0x83,8,0x04,0x84,64)
};
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) { (void)index;return configuration; }
uint16_t const *tud_descriptor_string_cb(uint8_t index,uint16_t language)
{
  (void)language;static uint16_t value[64];
  static char serial[PICO_UNIQUE_BOARD_ID_SIZE_BYTES*2+1];
  const char *strings[]={NULL,"Wireless UART prototype","Wireless UART Pico bridge",serial,"UART data","Bridge status"};
  if(index==0) { value[1]=0x0409;value[0]=(TUSB_DESC_STRING<<8)|4;return value; }
  if(index>=sizeof(strings)/sizeof(strings[0]))return NULL;
  if(index==3)pico_get_unique_board_id_string(serial,sizeof(serial));
  size_t n=strlen(strings[index]);if(n>63)n=63;
  for(size_t i=0;i<n;i++)value[i+1]=(uint8_t)strings[index][i];
  value[0]=(TUSB_DESC_STRING<<8)|(2*n+2);return value;
}
