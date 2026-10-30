/***************************************************************************//**
 * @author :  Engineer Akbar Shah
 * @file : app.c
 * @brief : Station firmware - transparent serial<->TCP bridge.
 *
 *  TCP_ECHO_TEST=1 tests the two radios without target UART wiring.
 *  TCP_ECHO_TEST=0 uses USART0 GPIO30 (TX) / GPIO29 (RX), at 921600 8N1.
 *  USB VCOM remains a separate diagnostic console in both modes.
 ******************************************************************************/

#include "cmsis_os2.h"
#include "sl_net.h"
#include "sl_board_configuration.h"
#include "sl_net_ping.h"
#include "sl_utility.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "sl_net_wifi_types.h"
#include "sl_net_default_values.h"
#include <string.h>

#include "rsi_debug.h"
#include "socket.h"
#include "errno.h"
#include "../wireless_receive.h"

#include "sl_si91x_core_utilities.h"
#include "target_uart.h"
#include "../diagnostic_source.h"
#include "target_control.h"

#define Firmware_Version "3.11.12-hb15s"
#define FIRMWARE_APP_TYPE "SiWx917 UART cable: target station / USART0"

/* Temporary bench mode: echo TCP input directly, without initializing USART0.
 * Change to 0 and rebuild/flash before testing the STM32 UART connection. */
#define TCP_ECHO_TEST 0


/* Station VCOM is diagnostic only; payload is on the separate USART0 pins. */
#define BRIDGE_VERBOSE 1

#if BRIDGE_VERBOSE
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) do { } while (0)
#endif

/******************************************************
 *                    Constants
 ******************************************************/
#define TCP_SERVER_IP "192.168.10.10"
#define TCP_SERVER_PORT 5005

/* Rejoin quickly; 100ms retries with a pinned channel failed to join (30 Oct). */
#define WIFI_RETRY_DELAY_MS 500
#define CONNECT_RETRY_DELAY_MS 500

#define TCP_RX_BUFFER_SIZE BRIDGE_FRAME_MAX

#define UART_RING_SIZE 24576
#define UART_TX_CHUNK BRIDGE_DATA_CHUNK

/******************************************************
 *               Variable Definitions
 ******************************************************/

static volatile int g_tcp_socket = -1;
volatile uint32_t bridge_tcp_tx_hash = 2166136261U;
volatile uint32_t bridge_tcp_sent_bytes;
static volatile bool g_wifi_connected = false;
static sl_wifi_device_configuration_t station_configuration;
volatile int32_t bridge_wifi_rssi;
volatile uint32_t bridge_wifi_rssi_status;
volatile uint32_t bridge_tcp_received_bytes;
volatile uint32_t bridge_uart_ring_overflows;
extern volatile uint32_t target_uart_rx_hash, target_uart_tx_hash;
extern volatile uint32_t target_uart_received_bytes, target_uart_sent_bytes;
#include "../bridge_diagnostics.h"
static void bridge_restart(uint32_t reason)
{
  bridge_save_failure(2U, reason, target_uart_received_bytes, target_uart_rx_hash,
      target_uart_sent_bytes, target_uart_tx_hash, target_uart_rx_errors,
      bridge_uart_ring_overflows, target_uart_tx_errors);
}

static volatile uint8_t uart_ring[UART_RING_SIZE];
static volatile uint32_t ring_head = 0;
static volatile uint32_t ring_tail = 0;

/* The SDK debug UART still references this symbol. Ignore VCOM input so it
 * cannot enter the target data stream. USART0 uses queue_target_block below. */
void cache_uart_rx_data(char character)
{
  UNUSED_PARAMETER(character);
}

static void queue_target_block(const uint8_t *data, uint32_t length)
{
  uint32_t head = ring_head;
  uint32_t free_bytes = (ring_tail + UART_RING_SIZE - head - 1U) % UART_RING_SIZE;
  uint32_t accepted = length < free_bytes ? length : free_bytes;
  bridge_uart_ring_overflows += length - accepted;
  if(accepted!=length) {
    if(bridge_diagnostic_mode)bridge_diagnostic_error=20U;
    bridge_trace_event(TRACE_UART_OVERFLOW,bridge_uart_ring_overflows,
        (head+UART_RING_SIZE-ring_tail)%UART_RING_SIZE,true);
    network_stop(20U,length-accepted);
  }
  uint32_t first = UART_RING_SIZE - head;
  if (first > accepted) first = accepted;
  memcpy((void *)&uart_ring[head], data, first);
  if (accepted > first) memcpy((void *)uart_ring, data + first, accepted - first);
  __DMB();
  ring_head = (head + accepted) % UART_RING_SIZE;
}

const osThreadAttr_t thread_attributes = {
    .name = "app",
    .attr_bits = 0,
    .cb_mem = 0,
    .cb_size = 0,
    .stack_mem = 0,
    .stack_size = 4096,
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
    .stack_size = 2048,
    .priority = osPriorityLow,
    .tz_module = 0,
    .reserved = 0,
};


/* Bridge: drain the ring and ship it in one batched send. */
static volatile bool baud_switching,baud_sender_busy;
static uint8_t apply_remote_baud(uint32_t baud,uint32_t *applied,uint32_t *driver)
{
  if(!baud) return target_uart_set_baud(0U,applied,driver);
  baud_switching=true;__DMB();
  uint32_t start=osKernelGetTickCount();
  while(baud_sender_busy) {
    if(osKernelGetTickCount()-start>=osKernelGetTickFreq()/2U) { baud_switching=false;*applied=*driver=0;return BAUD_BUSY; }
    osDelay(1U);
  }
  uint8_t result;
  if(ring_head!=ring_tail) { *applied=*driver=0;result=BAUD_BUSY; }
  else result=target_uart_set_baud(baud,applied,driver);
  __DMB();baud_switching=false;return result;
}

static void uart_bridge_task(void *argument)
{
  UNUSED_PARAMETER(argument);

  static uint8_t chunk[UART_TX_CHUNK];

  while (1)
  {
    if(bridge_full_reset_request)NVIC_SystemReset();
    if (g_tcp_socket < 0)
    {
      ring_tail = ring_head; /* discard: no peer */
      osDelay(5);
      continue;
    }

    if (!network_service(g_tcp_socket, ring_head != ring_tail || !target_uart_rx_idle())) {
      g_tcp_socket = -1;
      network_peer_closed = true;
      osDelay(1);
      continue;
    }

    if (baud_switching || ring_head == ring_tail)
    {
      osDelay(1);
      continue;
    }

    /* Batch to one TCP MSS or four milliseconds of actual UART idle.
     * Do not send one TCP packet on every scheduler tick during a stream. */
    baud_sender_busy=true;__DMB();
    if(baud_switching) { baud_sender_busy=false;osDelay(1U);continue; }
    uint32_t n = 0;
    while (n < UART_TX_CHUNK) {
      uint32_t tail = ring_tail;
      uint32_t head = ring_head;
      if (tail != head) {
        uint32_t available = (head + UART_RING_SIZE - tail) % UART_RING_SIZE;
        uint32_t take = UART_TX_CHUNK - n;
        if (take > available) take = available;
        uint32_t first = UART_RING_SIZE - tail;
        if (first > take) first = take;
        memcpy(chunk + n, (const void *)&uart_ring[tail], first);
        if (take > first) memcpy(chunk + n + first, (const void *)uart_ring, take - first);
        __DMB();
        ring_tail = (tail + take) % UART_RING_SIZE;
        n += take;
      } else if (target_uart_rx_idle()) {
        break;
      } else {
        osDelay(1U);
      }
    }

    if (n > 0)
    {
      uint32_t sent = 0;
      while (sent < n)
      {
        int local_socket = g_tcp_socket;
        if (local_socket < 0)
          break;
        if (!network_wait_payload_credit(local_socket, n - sent)) break;
        int r = network_send_frame(local_socket, BRIDGE_DATA, chunk + sent,
                                   (uint16_t)(n - sent), 0U);
        if (r <= 0)
        {
          bridge_network_last_errno = errno;
          g_tcp_socket = -1;
          network_peer_closed = true;
          break;
        }
        for (int i = 0; i < r; ++i)
          bridge_tcp_tx_hash = (bridge_tcp_tx_hash ^ chunk[sent + i]) * 16777619U;
        bridge_tcp_sent_bytes += (uint32_t)r;
        sent += (uint32_t)r;
      }
    }
    __DMB();baud_sender_busy=false;
  }
}

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
static sl_status_t network_event_handler(sl_net_event_t event, sl_status_t status, void *data, uint32_t data_length);
static sl_status_t join_fail_handler(sl_wifi_event_t event, sl_status_t status_code, void *data, uint32_t data_length, void *arg);

/******************************************************
 *               Function Definitions
 ******************************************************/
void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static sl_status_t join_fail_handler(sl_wifi_event_t event, sl_status_t status_code, void *data, uint32_t data_length, void *arg)
{
  network_record_station_event(1U,(uint32_t)event,(uint32_t)status_code,data,data_length);

  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event))
  {
    LOG("\r\nWi-Fi link lost\r\n");
    g_wifi_connected = false;
    network_peer_closed = true;
  }
  return SL_STATUS_OK;
}

static void wifi_connect_forever(void)
{
  sl_status_t status;

  while (!g_wifi_connected)
  {
    if(bridge_full_reset_request)NVIC_SystemReset();
    LOG("\r\nSearching for AP...\r\n");
    status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
    if (status == SL_STATUS_OK)
    {
      LOG("\r\nWi-Fi client connected\r\n");
      g_wifi_connected = true;
      break;
    }
    LOG("\r\nAP not found / join failed: 0x%lX, retrying in %dms\r\n",
        status, WIFI_RETRY_DELAY_MS);
    osDelay(WIFI_RETRY_DELAY_MS);
  }
}

static int tcp_connect_forever(void)
{
  int client_socket = -1;
  int ret = -1;
  // int attempt = 0;

  struct sockaddr_in server_addr = {0};
  socklen_t socket_len = sizeof(struct sockaddr_in);

  while (1)
  {
    if (!g_wifi_connected)
    {
      return -1;
    }

    client_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, network_receive_callback);
    if (client_socket < 0)
    {
      LOG("\r\nTCP client socket create failed, errno: %d\r\n", errno);
      osDelay(CONNECT_RETRY_DELAY_MS);
      continue;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = TCP_SERVER_PORT;
    sl_net_inet_addr(TCP_SERVER_IP, &server_addr.sin_addr.s_addr);

    // LOG("\r\n[attempt %d] Connecting to TCP server %s:%d...\r\n",
    //     attempt++, TCP_SERVER_IP, TCP_SERVER_PORT);

    if (network_socket_high_performance(client_socket) < 0) {
      bridge_network_last_errno = errno;
      close(client_socket);
      osDelay(CONNECT_RETRY_DELAY_MS);
      continue;
    }
    ret = connect(client_socket, (struct sockaddr *)&server_addr, socket_len);
    if (ret >= 0)
    {
      return client_socket;
    }

    LOG("\r\nTCP connect failed, errno: %d - retrying\r\n", errno);
    close(client_socket);
    client_socket = -1;
    osDelay(CONNECT_RETRY_DELAY_MS);
  }
}

static void application_start(void *argument)
{
  LOG("Firmware Version: %s\n\r", Firmware_Version);
  LOG("Firmware App Type: %s\n\r", FIRMWARE_APP_TYPE);
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  if (TCP_ECHO_TEST)
  {
    LOG("\r\nMODE: TCP ECHO TEST - target UART disabled; no pin wires required\r\n");
  }
  else
  {
    if (!target_uart_init(queue_target_block))
    {
      LOG("\r\nTarget USART0 init failed: %ld\r\n", (long)target_uart_last_status);
      return;
    }
    target_control_init();
    network_baud_handler=apply_remote_baud;
    network_control_handler = target_control_execute;
    network_disconnect_handler = target_control_release;
    LOG("\r\nTarget UART ready: GPIO30 TX, GPIO29 RX, 921600 8N1\r\n");
    osThreadNew((osThreadFunc_t)uart_bridge_task, NULL, &uart_bridge_thread_attributes);
  }

  /* The SDK documents BI_DIR_ACK_UPDATE as required for bidirectional TCP.
   * WINDOW_DIV activates the ACK-frequency tuning configured for our sockets.
   * The generic MCU client default omits both and also disables aggregation. */
  station_configuration = sl_wifi_default_client_configuration;
  station_configuration.boot_config.ext_tcp_ip_feature_bit_map |=
      SL_SI91X_EXT_TCP_IP_BI_DIR_ACK_UPDATE | SL_SI91X_EXT_TCP_IP_WINDOW_DIV;
  station_configuration.boot_config.feature_bit_map |= SL_WIFI_FEAT_AGGREGATION;
  station_configuration.boot_config.custom_feature_bit_map |= SL_SI91X_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ;
  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_configuration, NULL, network_event_handler);
  if (status != SL_STATUS_OK)
  {
    LOG("\r\nFailed to start Wi-Fi Client interface: 0x%lX\r\n", status);
    return;
  }
  LOG("\r\nWi-Fi client interface init success\r\n");


  if (!network_initialize_policy(false)) {
    LOG("Wi-Fi performance policy failed: 0x%lX\r\n", bridge_power_status);
    return;
  }
  LOG("NWP %u.%u.%u.%u.%u.%u; high-performance profile=%lu\r\n",
      bridge_nwp_firmware.major, bridge_nwp_firmware.minor,
      bridge_nwp_firmware.security_version, bridge_nwp_firmware.patch_num,
      bridge_nwp_firmware.customer_id, bridge_nwp_firmware.build_num, bridge_power_profile);
  sl_wifi_set_callback_v2(SL_WIFI_JOIN_EVENTS, join_fail_handler, NULL);

  uint8_t rx_buf[TCP_RX_BUFFER_SIZE];
  int client_socket = -1;
  bool socket_buffers_ready = false;

  while (1)
  {
    wifi_connect_forever();
    if (!network_initialize_policy(true)) {
      LOG("Applying high-performance mode failed: 0x%lX\r\n", bridge_power_status);
      return;
    }
    LOG("High-performance NWP command accepted; profile=%lu\r\n", bridge_power_profile);
    bridge_wifi_rssi_status = sl_wifi_get_signal_strength(SL_WIFI_CLIENT_INTERFACE, (int32_t *)&bridge_wifi_rssi);
    LOG("Wi-Fi RSSI: %ld dBm (status 0x%lX)\r\n", (long)bridge_wifi_rssi, bridge_wifi_rssi_status);
    if (!socket_buffers_ready) {
      status = network_socket_configure();
      LOG("Socket buffer configuration status: 0x%lX\r\n", status);
      if (status != SL_STATUS_OK) return;
      socket_buffers_ready = true;
    }

    network_receive_reset();
    client_socket = tcp_connect_forever();
    if (client_socket < 0)
    {
      sl_net_down(SL_NET_WIFI_CLIENT_INTERFACE);
      continue;
    }

    if(!network_credit_open(client_socket,false)) {bridge_restart(2U);return;}
    LOG("\r\nTCP connected to AP server. Bridge is live.\r\n");

    ring_tail = ring_head; /* flush stale bytes before going live */
    network_peer_closed = false;
    network_last_rx_tick = network_last_tx_tick = osKernelGetTickCount();
    bridge_stop_reason = bridge_stop_detail = 0U;
      g_tcp_socket = client_socket;

    /* Bench mode echoes TCP directly; normal mode writes the external UART. */
    while (1)
    {
      int len = network_receive(rx_buf, sizeof(rx_buf));
      if (len == 0) { osDelay(1); continue; }

      if (len > 0)
      {
        bridge_tcp_received_bytes += (uint32_t)len;
        bool delivered = true;
        if (TCP_ECHO_TEST)
        {
          int sent = 0;
          while (sent < len)
          {
            int result = network_send_frame(client_socket, BRIDGE_DATA, rx_buf + sent, (uint16_t)(len - sent), 0U);
            if (result <= 0)
            {
              LOG("\r\nTCP echo send failed, errno: %d\r\n", errno);
              delivered = false;
              break;
            }
            sent += result;
          }
          if (delivered)
          {
            LOG("\r\nTCP echo: returned %d bytes\r\n", len);
          }
        }
        else if(bridge_diagnostic_mode==2U) { /* Diagnostic accepts forward DATA without target echo. */ }
        else if (!target_uart_write(rx_buf, (uint32_t)len))
        {
          network_stop(11U, (uint32_t)len);
          LOG("\r\nTarget UART TX failed: %ld\r\n", (long)target_uart_last_status);
          delivered = false;
        }
        if (!delivered)
        {
          g_tcp_socket = -1;
          break;
        }
        network_payload_delivered((uint32_t)len);
      }
      else
      {
        g_tcp_socket = -1;
        break;
      }
    }

    /* Restart association after any failed session, including half-open TCP.
     * g_wifi_connected alone cannot detect an AP that rebooted silently. */
    network_peer_closed = true;
    target_control_release();
    g_wifi_connected = false;
    /* Avoid the SDK's unbounded close wait after losing an established
     * session. Release target control pins before restarting the radio. */
    osDelay(20);
    bridge_restart(2U);
    /* SDK shutdown waits without a timeout for its TX queue to empty.
     * On a dead peer, disconnect the VAP first: the SDK disconnect handler
     * flushes pending queues and marks the socket INTERFACE_DOWN. Then close
     * frees it without waiting for delivery to the missing AP. */
    sl_net_down(SL_NET_WIFI_CLIENT_INTERFACE);
    close(client_socket);
    client_socket = -1;

    osDelay(500);
  }
}

static sl_status_t network_event_handler(sl_net_event_t event, sl_status_t status, void *data, uint32_t data_length)
{
  if (event != SL_NET_PING_RESPONSE_EVENT)
    network_record_station_event(2U,(uint32_t)event,(uint32_t)status,data,data_length);

  UNUSED_PARAMETER(data_length);
  switch (event)
  {
  case SL_NET_PING_RESPONSE_EVENT:
  {
    sl_net_ping_response_t *response = (sl_net_ping_response_t *)data;
    UNUSED_PARAMETER(response);
    if (status != SL_STATUS_OK)
    {
      LOG("\r\nPing request failed!\r\n");
      return status;
    }
    LOG("\r\n%u bytes received from %u.%u.%u.%u\r\n",
        response->ping_size,
        response->ping_address.ipv4_address[0],
        response->ping_address.ipv4_address[1],
        response->ping_address.ipv4_address[2],
        response->ping_address.ipv4_address[3]);
    break;
  }
  case SL_NET_DHCP_NOTIFICATION_EVENT:
  {
    LOG("\r\nReceived DHCP Notification event with status : 0x%lX\r\n", status);
    break;
  }
  case SL_NET_IP_ADDRESS_CHANGE_EVENT:
  {
    sl_net_ip_configuration_t *ip_config = (sl_net_ip_configuration_t *)data;
    UNUSED_PARAMETER(ip_config);
    LOG("\r\nReceived Ip Address Change Notification event with status : 0x%lX\r\n", status);
    LOG("\t Ip Address : %u.%u.%u.%u\r\n",
        ip_config->ip.v4.ip_address.bytes[0],
        ip_config->ip.v4.ip_address.bytes[1],
        ip_config->ip.v4.ip_address.bytes[2],
        ip_config->ip.v4.ip_address.bytes[3]);
    break;
  }
  default:
    break;
  }

  return SL_STATUS_OK;
}
