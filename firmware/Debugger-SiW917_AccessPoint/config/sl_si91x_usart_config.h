/***************************************************************************//**
 * @author :  Engineer Akbar Shah
 * @date   :  2024-06-10
 * @file   :  sl_si91x_usart_config.h
 * @brief  :        Defines the Si91x USART configuration for the project.
 ******************************************************************************/



#ifndef SL_SI91X_USART_CONFIG_H
#define SL_SI91X_USART_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_si91x_usart.h"
#include "sl_si91x_usart_common_config.h"

// <<< Use Configuration Wizard in Context Menu >>>

//  <e>USART UC Configuration
//  <i> Enable: Peripheral configuration is taken straight from the configuration set in the universal configuration (UC).
//  <i> Disable: If the application demands it to be modified during runtime, use the sl_si91x_usart_set_configuration API to modify the peripheral configuration.
//  <i> Default: 1
#define USART_UC 1

// <h>USART Configuration
#define ENABLE  1
#define DISABLE 0

#define USART0_MODULE 0

#if (USART_UC)
// <o SL_USART_BAUDRATE> Baud Rate (Baud/Second) <300-7372800>
// <i> Default: 115200
#define SL_USART_BAUDRATE 921600

// <o SL_USART_MODE> Mode
// <SL_USART_MODE_ASYNCHRONOUS=> Asynchronous
// <SL_USART_MODE_SYNCHRONOUS_MASTER=> Synchronous Master
// <SL_USART_MODE_SYNCHRONOUS_SLAVE=> Synchronous Slave
// <SL_USART_MODE_SINGLE_WIRE=> Single Wire
// <SL_USART_MODE_IRDA=> IRDA
// <i> Default: SL_USART_MODE_ASYNCHRONOUS
#define SL_USART_MODE SL_USART_MODE_SYNCHRONOUS_MASTER

// <o SL_USART_PARITY> Parity
// <SL_USART_NO_PARITY=> No Parity
// <SL_USART_EVEN_PARITY=> Even
// <SL_USART_ODD_PARITY=> Odd
// <i> Default: SL_USART_NO_PARITY
#define SL_USART_PARITY SL_USART_NO_PARITY

// <o SL_USART_STOP_BITS> Stop Bits
// <SL_USART_STOP_BITS_1=> 1
// <SL_USART_STOP_BITS_1_5=> 1.5
// <SL_USART_STOP_BITS_2=> 2
// <i> Default: USART_STOP_BITS_1
#define SL_USART_STOP_BITS SL_USART_STOP_BITS_1

// <o SL_USART_DATA_BITS> Data Width
// <SL_USART_DATA_BITS_5=> 5
// <SL_USART_DATA_BITS_6=> 6
// <SL_USART_DATA_BITS_7=> 7
// <SL_USART_DATA_BITS_8=> 8
// <i> Default: USART_DATA_BITS_8
#define SL_USART_DATA_BITS SL_USART_DATA_BITS_8

// <o SL_USART_FLOW_CONTROL_TYPE> Flow control
// <ARM_USART_FLOW_CONTROL_NONE=> None
// <ARM_USART_FLOW_CONTROL_RTS_CTS=> CTS/RTS
// <i> Default: ARM_USART_FLOW_CONTROL_NONE
#define SL_USART_FLOW_CONTROL_TYPE ARM_USART_FLOW_CONTROL_NONE
#endif

// </h>  USART Configuration
// </e>

#ifdef __cplusplus
}
#endif
// <<< end of configuration section >>>

#if (SL_USART_FLOW_CONTROL_TYPE == ARM_USART_FLOW_CONTROL_RTS_CTS)
#if !defined(SL_USART0_CTS_PIN) && !defined(SL_USART0_RTS_PIN)
#warning "Select CTS, RTS gpio pins"
#endif
#endif

#if (USART_UC)
sl_si91x_usart_control_config_t usart_configuration = { .baudrate      = SL_USART_BAUDRATE,
                                                        .mode          = SL_USART_MODE,
                                                        .parity        = SL_USART_PARITY,
                                                        .stopbits      = SL_USART_STOP_BITS,
                                                        .hwflowcontrol = SL_USART_FLOW_CONTROL_TYPE,
                                                        .databits      = SL_USART_DATA_BITS,
                                                        .usart_module  = USART0_MODULE };
#endif

#endif //SL_SI91X_USART_CONFIG_H

