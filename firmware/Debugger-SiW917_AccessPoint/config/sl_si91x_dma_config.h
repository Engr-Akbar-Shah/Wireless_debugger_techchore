/***************************************************************************//**
 * @author :  Engineer Akbar Shah
 * @date   :  2024-06-10
 * @file   :  sl_si91x_dma_config.h
 * @brief  :        Contains Si91x DMA configuration settings.
 ******************************************************************************/



#ifndef SL_SI91X_DMA_CONFIG_H
#define SL_SI91X_DMA_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
#ifdef __cplusplus
extern "C" {
#endif

// <o SL_DMA0_CHANNEL_COUNT> DMA instance 0 channel count <1-32>
// <i> Default: 32
#define SL_DMA0_CHANNEL_COUNT 32

// <o SL_ULP_DMA_CHANNEL_COUNT> ULP DMA instance channel count <1-12>
// <i> Default: 12
#define SL_ULP_DMA_CHANNEL_COUNT 12

// <<< end of configuration section >>>

#ifdef __cplusplus
}
#endif
#endif //SL_SI91X_DMA_CONFIG_H

