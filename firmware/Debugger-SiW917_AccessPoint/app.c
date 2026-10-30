/***************************************************************************/ /**
                                                                               * @author :  Engineer Akbar Shah
                                                                               * @date   :  2026-07-17
                                                                               * @file   :  app.c
                                                                               * @brief  :  PC baud-test variant: framed USB data/control to TCP.
                                                                               *
                                                                               *  This board sits on the PC USB port. The Linux baud-test application uses
                                                                               *  the VCOM serial port at fixed 1M 8N1. Payload is framed separately from
                                                                               *  baud requests. The matching B image uses radio protocol version3.
                                                                               *
                                                                               *  CRITICAL: the VCOM UART is now a DATA PIPE, not a console. There must be
                                                                               *  ZERO printf on this board - any debug text would be injected straight
                                                                               *  into the serial byte stream and corrupt the payload. All application printf
                                                                               *  calls are compiled out via the LOG() macro below. Do not add printf.
                                                                               ******************************************************************************/

#include "cmsis_os2.h"
#include "sl_net.h"
#include "sl_utility.h"
#include "sl_wifi_callback_framework.h"

#include "rsi_debug.h"
#include "USART.h"
#include "rsi_udma.h"
#include "rsi_ulpss_clk.h"
#include "rsi_rom_ulpss_clk.h"
#include "clock_update.h"

#include "errno.h"
#include "../wireless_receive.h"
#include "socket.h"
#include <string.h>

#include "sl_si91x_core_utilities.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_socket_utility.h"

#include "sl_net_wifi_types.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"

#define Firmware_Version "3.11.12-hb15s"
#define FIRMWARE_APP_TYPE                                                      \
  "SiWx917 UART cable: laptop AP / USB VCOM"

/* Bridge must be silent on the VCOM UART. Set to 1 ONLY for bench debugging
 * WITHOUT a serial payload test attached. */
#define BRIDGE_VERBOSE 0

/* GCC/newlib output is discarded, including SDK printf calls. Payload bytes
 * use the UART driver directly. Linked with --wrap=_write. */
int __wrap__write(int file, const char *data, int length)
{
  (void)file;
  (void)data;
  return length;
}

#if BRIDGE_VERBOSE
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)                                                               \
  do {                                                                         \
  } while (0)
#endif

/******************************************************
 *               Socket configuration
 ******************************************************/
#define TOTAL_SOCKETS 2
#define TOTAL_TCP_SOCKETS 2
#define TOTAL_UDP_SOCKETS 0
#define TCP_TX_ONLY_SOCKETS 0
#define TCP_RX_ONLY_SOCKETS 2
#define UDP_TX_ONLY_SOCKETS 0
#define UDP_RX_ONLY_SOCKETS 0
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS 0
#define TCP_RX_WINDOW_SIZE_CAP 44
#define TCP_RX_WINDOW_DIV_FACTOR 44

#define TCP_SERVER_PORT 5005
#define TCP_RX_BUFFER_SIZE BRIDGE_FRAME_MAX

/* serial -> tcp coalescing */
#define UART_RING_SIZE 8192
#define UART_TX_CHUNK BRIDGE_DATA_CHUNK

/******************************************************
 *   Device configuration (unchanged from v1)
 ******************************************************/
static const sl_wifi_device_configuration_t ap_device_configuration = {
    .boot_option = LOAD_NWP_FW,
    .mac_address = NULL,
    .band = SL_SI91X_WIFI_BAND_2_4GHZ,
    .region_code = US,
    .boot_config =
        {.oper_mode = SL_SI91X_ACCESS_POINT_MODE,
         .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
         .feature_bit_map =
             (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_AGGREGATION |
              SL_WIFI_FEAT_WPS_DISABLE),
         .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_SERVER |
                                    SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
         .custom_feature_bit_map = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID |
                                    SL_SI91X_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ),
         .ext_custom_feature_bit_map =
             (MEMORY_CONFIG
#ifdef SLI_SI917
              | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
              ),
         .bt_feature_bit_map = 0,
         .ext_tcp_ip_feature_bit_map =
             (SL_SI91X_EXT_TCP_IP_BI_DIR_ACK_UPDATE | SL_SI91X_EXT_TCP_IP_WINDOW_DIV),
         .ble_feature_bit_map = 0,
         .ble_ext_feature_bit_map = 0,
         .config_feature_bit_map = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP},
    .ta_pool = {.tx_ratio_in_buffer_pool = 1,
                .rx_ratio_in_buffer_pool = 1,
                .global_ratio_in_buffer_pool = 1}};

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* The active connected TCP socket. -1 when no station is connected. */
static volatile int g_tcp_socket = -1;
volatile uint32_t bridge_tcp_tx_hash = 2166136261U;
volatile uint32_t bridge_tcp_sent_bytes;

/* serial rx ring buffer: reader task fills it, bridge task drains it */
static volatile uint8_t uart_ring[UART_RING_SIZE];
static volatile uint32_t ring_head = 0; /* write index (reader task) */
static volatile uint32_t ring_tail = 0; /* read index (bridge task)  */

#include "host_uart.h"
volatile uint32_t bridge_network_stage,bridge_network_status;
volatile uint32_t bridge_vcom_ring_overflows;
#include "../bridge_diagnostics.h"
extern volatile uint32_t pc_bad_frames,host_uart_rx_dma_stalls,host_uart_rx_dma_blocks;
__attribute__((section(".noinit"))) volatile uint32_t pc_failure_debug[8];
static void pc_save_return_stats(void);
static void bridge_restart(uint32_t reason)
{
  pc_save_return_stats();
  pc_failure_debug[0]=0x50434442U;
  pc_failure_debug[1]=pc_bad_frames;
  pc_failure_debug[2]=host_uart_rx_dma_stalls;
  pc_failure_debug[3]=host_uart_rx_dma_blocks;
  pc_failure_debug[4]=bridge_vcom_received_bytes;
  pc_failure_debug[5]=bridge_vcom_rx_errors;
  pc_failure_debug[6]=UART0->LSR;
  pc_failure_debug[7]=(ring_head+UART_RING_SIZE-ring_tail)%UART_RING_SIZE;
  bridge_save_failure(1U,reason,bridge_vcom_received_bytes,bridge_vcom_rx_hash,
      bridge_vcom_sent_bytes,bridge_vcom_tx_hash,bridge_vcom_rx_errors,
      bridge_vcom_ring_overflows,bridge_vcom_tx_errors);
}
static void queue_payload_block(const uint8_t *data,uint32_t length)
{
  uint32_t head=ring_head;
  uint32_t free=(ring_tail+UART_RING_SIZE-head-1U)%UART_RING_SIZE;
  uint32_t accepted=length<free ? length : free;
  bridge_vcom_ring_overflows+=length-accepted;
  uint32_t first=UART_RING_SIZE-head;if(first>accepted) first=accepted;
  memcpy((void *)&uart_ring[head],data,first);
  if(accepted>first) memcpy((void *)uart_ring,data+first,accepted-first);
  __DMB();ring_head=(head+accepted)%UART_RING_SIZE;
}
#include "../pc_host.h"


/* SDK debug callback is deliberately separate from the Pico payload UART. */
void cache_uart_rx_data(char character) { (void)character; }

const osThreadAttr_t thread_attributes = {
    .name = "app",
    .attr_bits = 0,
    .cb_mem = 0,
    .cb_size = 0,
    .stack_mem = 0,
    .stack_size = 3072,
    .priority = osPriorityLow,
    .tz_module = 0,
    .reserved = 0,
};

const osThreadAttr_t uart_bridge_thread_attributes = {
    .name = "uart_br",
    .attr_bits = 0,
    .cb_mem = 0,
    .cb_size = 0,
    .stack_mem = 0,
    .stack_size = 3072,
    .priority = osPriorityLow,
    .tz_module = 0,
    .reserved = 0,
};

const osThreadAttr_t tcp_server_thread_attributes = {
    .name = "tcp_server",
    .attr_bits = 0,
    .cb_mem = 0,
    .cb_size = 0,
    .stack_mem = 0,
    .stack_size = 3072,
    .priority = osPriorityLow,
    .tz_module = 0,
    .reserved = 0,
};




/* Task 2: wait for data, let a few more bytes pile up, ship the whole
 * chunk in one TCP send. Batching matters - one send() per byte would
 * choke the NWP command interface. */
static void uart_bridge_task(void *argument) {
  UNUSED_PARAMETER(argument);
  while(1) {
    if(bridge_full_reset_request)NVIC_SystemReset();
    int socket_id=g_tcp_socket;
    if(socket_id>=0 && !network_service(socket_id,pc_payload_can_piggyback_credit())) {
      g_tcp_socket=-1;network_peer_closed=true;
    }
    if(!pc_host_service(g_tcp_socket)) {
      network_stop(15U,0U);osDelay(20U);bridge_restart(1U);
    }
    if(network_peer_closed && socket_id>=0) {
      osDelay(20U);bridge_restart(1U);
    }
    osDelay(1U);
  }
}

volatile uint32_t bridge_listener_stage, bridge_listener_retries;
static int tcp_open_listener(void)
{
  bridge_listener_stage = 1U;
  int fd = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, network_receive_callback);
  if (fd < 0) return -1;
  uint8_t vap = SL_SI91X_WIFI_AP_VAP_ID;
  struct sockaddr_in address = {0};
  address.sin_family = AF_INET;
  address.sin_port = TCP_SERVER_PORT;
  bridge_listener_stage = 2U;
  if (network_socket_high_performance(fd) < 0) goto failed;
  bridge_listener_stage = 3U;
  if (setsockopt(fd, SOL_SOCKET, SL_SO_SOCK_VAP_ID, &vap, sizeof(vap)) < 0) goto failed;
  bridge_listener_stage = 4U;
  if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) goto failed;
  bridge_listener_stage = 5U;
  if (listen(fd, 1) < 0) goto failed;
  return fd;
failed:
  bridge_network_last_errno = errno;
  close(fd);
  return -1;
}
static void tcp_server_task(void *argument)
{
  UNUSED_PARAMETER(argument);
  static uint8_t rx_buf[TCP_RX_BUFFER_SIZE];
  if (network_socket_configure() != SL_STATUS_OK) return;
  while (1) {
    network_receive_reset();
    int server_socket = tcp_open_listener();
    if (server_socket < 0) {
      bridge_listener_retries++;
      osDelay(1000);
      continue;
    }
    bridge_listener_stage = 6U;
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket >= 0) {
      if(!network_credit_open(client_socket,true)) {bridge_restart(2U);return;}
      if (bridge_control_request || bridge_control_status == 1U) bridge_control_status = 3U;
      bridge_control_request = 0; /* Reject commands queued while disconnected. */
      network_peer_closed = false;
      network_last_rx_tick = network_last_tx_tick = osKernelGetTickCount();
      bridge_stop_reason = bridge_stop_detail = 0U;
      g_tcp_socket = client_socket;
      bridge_listener_stage = 7U;
      while (1) {
        int len = network_receive(rx_buf, sizeof(rx_buf));
        if (len == 0) { osDelay(1); continue; }
        if (len < 0) break;
        if (!payload_uart_write(rx_buf, (uint32_t)len)) {
          network_stop(11U, (uint32_t)len); break;
        }
        network_payload_delivered((uint32_t)len);
      }
      g_tcp_socket = -1;
      network_peer_closed = true;
      /* A remote close can arrive before the sender notices the failure.
       * This receive path must also bypass the SDK's unbounded close. */
      osDelay(20);
      bridge_restart(2U);
    }
    /* The first client shares the listener's NWP ID in SDK 4.0.2.
     * Recreate the listener, reapplying its performance settings each session. */
    /* SDK close(listener) uses SHUTDOWN_BY_PORT and frees the accepted
     * child too. Do not close the shared first-client NWP ID beforehand. */
    close(server_socket);
    bridge_listener_retries++;
    osDelay(250);
  }
}

/******************************************************
 *               Function Declarations
 ******************************************************/

static sl_status_t ap_connected_event_handler(sl_wifi_event_t event,
                                              sl_status_t status_code,
                                              void *data, uint32_t data_length,
                                              void *arg);
static sl_status_t
ap_disconnected_event_handler(sl_wifi_event_t event, sl_status_t status_code,
                              void *data, uint32_t data_length, void *arg);
static void application_start(void *argument);

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(void) {
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument) {
  pc_output_mutex=osMutexNew(NULL);
  pc_return_mutex=osMutexNew(NULL);
  if(!pc_output_mutex || !pc_return_mutex) return;
  bridge_network_stage = 1U;
  LOG("Firmware Version: %s\n\r", Firmware_Version);
  LOG("Firmware App Type: %s\n\r", FIRMWARE_APP_TYPE);

  UNUSED_PARAMETER(argument);
  sl_status_t status;

  bridge_network_stage = 2U;
  status = sl_net_init(SL_NET_WIFI_AP_INTERFACE, &ap_device_configuration, NULL,
                       NULL);
  bridge_network_status = status;
  if (status != SL_STATUS_OK) {
    LOG("\r\nFailed to start Master Wi-Fi interface: 0x%lx\r\n", status);
    return;
  }

  sl_wifi_set_callback_v2(SL_WIFI_CLIENT_CONNECTED_EVENTS,
                          ap_connected_event_handler, NULL);
  sl_wifi_set_callback_v2(SL_WIFI_CLIENT_DISCONNECTED_EVENTS,
                          ap_disconnected_event_handler, NULL);

  if (!network_initialize_policy(false)) return;

  /* This pair does not roam. Avoid automatic channel selection on every
   * restart, and advertise the same 2.4 GHz/20 MHz channel each time. */
  sl_net_wifi_ap_profile_t ap_profile;
  status = sl_net_get_profile(SL_NET_WIFI_AP_INTERFACE,
                             SL_NET_DEFAULT_WIFI_AP_PROFILE_ID, &ap_profile);
  if (status != SL_STATUS_OK) { bridge_network_status = status; return; }
  bridge_ap_keepalive_before = ap_profile.config.keepalive_type;
  bridge_ap_idle_before = ap_profile.config.client_idle_timeout;
  /* Framed TCP heartbeats own peer health. Avoid a second NULL-probe
   * policy deauthenticating this busy point-to-point station. */
  ap_profile.config.keepalive_type = SL_WIFI_AP_KEEP_ALIVE_DISABLE;
  ap_profile.config.client_idle_timeout = 0U;
  bridge_ap_keepalive_applied = ap_profile.config.keepalive_type;
  ap_profile.config.channel.channel = 6U;
  ap_profile.config.channel.band = SL_WIFI_BAND_2_4GHZ;
  ap_profile.config.channel.bandwidth = SL_WIFI_BANDWIDTH_20MHz;
  status = sl_net_set_profile(SL_NET_WIFI_AP_INTERFACE,
                             SL_NET_DEFAULT_WIFI_AP_PROFILE_ID, &ap_profile);
  if (status != SL_STATUS_OK) { bridge_network_status = status; return; }

  bridge_network_stage = 3U;
  status =
      sl_net_up(SL_NET_WIFI_AP_INTERFACE, SL_NET_DEFAULT_WIFI_AP_PROFILE_ID);
  bridge_network_status = status;
  if (status != SL_STATUS_OK) {
    LOG("\r\nFailed to bring Master Wi-Fi interface up: 0x%lx\r\n", status);
    return;
  }

  if(!host_uart_init(queue_payload_block)) return;
  const osThreadAttr_t output_attributes={
    .name="pc_out",.stack_size=2048U,.priority=osPriorityLow
  };
  if(!osThreadNew(pc_output_worker,NULL,&output_attributes))return;
  if (osThreadNew((osThreadFunc_t)tcp_server_task, NULL,
                  &tcp_server_thread_attributes) == NULL) return;
  bridge_network_stage = 4U;
  osThreadNew((osThreadFunc_t)uart_bridge_task, NULL,
              &uart_bridge_thread_attributes);

  /* Local capacity investigation is complete. Production does not create
   * its diagnostic task or retain its1KiB pattern buffer in the image. */
  uint32_t last_peer_tick = osKernelGetTickCount();
  while (1) {
    if (g_tcp_socket >= 0) {
      last_peer_tick = osKernelGetTickCount();
      /* Independent of a sender blocked inside an SDK call. Normal peers
       * send a heartbeat each second, even when the serial terminal is idle. */
      if (network_elapsed(network_last_rx_tick, 30000U)) bridge_restart(4U);
    } else if (network_elapsed(last_peer_tick, 60000U)) {
      /* A failed test left accept() waiting indefinitely while neither B
       * nor a laptop scan could see the AP. Retry full initialization instead
       * of treating a successful sl_net_up() as perpetual RF availability.
       * J-Link is independent, so this does not remove the laptop COM port. */
      bridge_restart(3U);
    }
    osDelay((osKernelGetTickFreq() + 9U) / 10U);
  }
}

static sl_status_t ap_connected_event_handler(sl_wifi_event_t event,
                                              sl_status_t status_code,
                                              void *data, uint32_t data_length,
                                              void *arg) {
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    return status_code;
  }




  bridge_ap_connect_events++;
  bridge_ap_event_id = (uint32_t)event;
  bridge_ap_event_tick = osKernelGetTickCount();
  if (data && data_length >= 6U) {
    const uint8_t *mac = data;
    bridge_ap_connected_mac_lo = mac[0] | ((uint32_t)mac[1]<<8) | ((uint32_t)mac[2]<<16) | ((uint32_t)mac[3]<<24);
    bridge_ap_connected_mac_hi = mac[4] | ((uint32_t)mac[5]<<8);
  }
  return SL_STATUS_OK;
}

static sl_status_t
ap_disconnected_event_handler(sl_wifi_event_t event, sl_status_t status_code,
                              void *data, uint32_t data_length, void *arg) {
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    return status_code;
  }

  bridge_ap_disconnect_events++;
  bridge_ap_event_id = (uint32_t)event;
  bridge_ap_event_tick = osKernelGetTickCount();
  if (data && data_length >= 6U) {
    const uint8_t *mac = data;
    bridge_ap_disconnected_mac_lo = mac[0] | ((uint32_t)mac[1]<<8) | ((uint32_t)mac[2]<<16) | ((uint32_t)mac[3]<<24);
    bridge_ap_disconnected_mac_hi = mac[4] | ((uint32_t)mac[5]<<8);
  }
  /* This event is not a TCP closure and may refer to another AP client.
   * Socket termination and the framed heartbeat retain bounded recovery. */
  return SL_STATUS_OK;
}
