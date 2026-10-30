/***************************************************************************//**
 * @author :  Engineer Akbar Shah
 * @date: 2024-06-10
 * @file : sl_si91x_usart_common_config.h
 * @brief : Configuration definitions for the SiW917 station project.
 ******************************************************************************/

#ifndef SL_SI91X_USART_COMMON_CONFIG_H
#define SL_SI91X_USART_COMMON_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>USART0 Configuration

// <q SL_USART0_DMA_CONFIG_ENABLE> USART0 DMA
// <i> Default: 1
#define SL_USART0_DMA_CONFIG_ENABLE 1

// <q SL_USART0_SYNC_MODE_EN> USART0 SYNC Mode
// <i> Default: 0
#define SL_USART0_SYNC_MODE_EN 0

// <o SL_USART0_CLOCK_SRC> USART0 Clock Source
// <USART_ULPREFCLK=> ULP REF CLK
// <USART_SOCPLLCLK=> SOC PLL CLK
// <USART_INTFPLLCLK=> INTF PLL CLK
// <i> Default: USART_ULPREFCLK
#define SL_USART0_CLOCK_SRC USART_INTFPLLCLK

// <o SL_USART0_FRAC_DIV_SEL> USART0 SCLK fractional divider mode
// <USART_FRACTIONAL_DIVIDER=> Fractional (clk_in / (div + 0.5))
// <USART_CLOCK_SWALLOW=> Clock swallow (clk_in / div)
// <i> See USART_SCLK_FRAC_SEL_T in rsi_pll.h
// <i> Default: USART_FRACTIONAL_DIVIDER (matches typical RTE_Device_917.h; use USART_CLOCK_SWALLOW in UC when needed)
#define SL_USART0_FRAC_DIV_SEL USART_CLOCK_SWALLOW

// </h>
// <<< end of configuration section >>>

/* RTE_USART0_FRAC_DIV_SEL follows UC (SL_USART0_FRAC_DIV_SEL); RTE_Device value is not used after this header. */
#ifdef RTE_USART0_FRAC_DIV_SEL
#undef RTE_USART0_FRAC_DIV_SEL
#endif
#define RTE_USART0_FRAC_DIV_SEL SL_USART0_FRAC_DIV_SEL

// <<< sl:start pin_tool >>>
// <usart0 signal=(CLK),TX,RX,(CTS),(RTS)> SL_USART0
// $[USART0_SL_USART0]
#ifndef SL_USART0_PERIPHERAL                    
#define SL_USART0_PERIPHERAL                     USART0
#endif
#ifndef SL_USART0_PERIPHERAL_NO                 
#define SL_USART0_PERIPHERAL_NO                  0
#endif


// USART0 TX on GPIO_30
#ifndef SL_USART0_TX_PORT                       
#define SL_USART0_TX_PORT                        HP
#endif
#ifndef SL_USART0_TX_PIN                        
#define SL_USART0_TX_PIN                         30
#endif
#ifndef SL_USART0_TX_LOC                        
#define SL_USART0_TX_LOC                         5
#endif

// USART0 RX on GPIO_29
#ifndef SL_USART0_RX_PORT                       
#define SL_USART0_RX_PORT                        HP
#endif
#ifndef SL_USART0_RX_PIN                        
#define SL_USART0_RX_PIN                         29
#endif
#ifndef SL_USART0_RX_LOC                        
#define SL_USART0_RX_LOC                         10
#endif


// [USART0_SL_USART0]$
// <<< sl:end pin_tool >>>
#endif // SL_SI91X_USART_COMMON_CONFIG_H
