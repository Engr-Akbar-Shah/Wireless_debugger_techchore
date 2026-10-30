/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
  uint8_t data[256];
  volatile uint16_t head;
  volatile uint16_t tail;
} RxQueue;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* 1: USB USART2 <-> external USART1 for the D8-D2 jumper test.
 * 0: echo USART1 payload back to USART1 for the later SiWx917 test.
 * Remove the D8-D2 jumper BEFORE selecting mode 0. */
#define LOCAL_TEST 0

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
static RxQueue target_queue;
static RxQueue usb_queue;
static uint8_t target_byte;
static uint8_t usb_byte;
#define TARGET_DMA_RX_SIZE 16384U
static uint8_t target_dma_rx[TARGET_DMA_RX_SIZE];
static uint32_t target_dma_read;
static uint32_t target_dma_consumed;
volatile uint32_t target_dma_rx_wraps, target_dma_produced_bytes;
volatile uint32_t target_dma_overrun_bytes, target_dma_backpressure;
volatile uint32_t target_uart_brr;
volatile uint32_t target_fixture_version = 0x00030800U;
volatile uint32_t target_dma_received_bytes;
volatile uint32_t target_dma_rx_hash = 2166136261U;
volatile uint32_t target_dma_errors;
volatile uint32_t uart_overrun_errors;
volatile uint32_t uart_framing_errors;
volatile uint32_t uart_noise_errors;
volatile uint32_t uart_last_error_flags;
#define TARGET_DMA_TX_SIZE 16384U
static uint8_t target_dma_tx[TARGET_DMA_TX_SIZE];
static volatile uint32_t target_tx_head, target_tx_tail, target_tx_active;
volatile uint32_t target_dma_transmitted_bytes, target_dma_tx_errors, target_dma_tx_queue_peak;
volatile uint32_t target_dma_tx_hash = 2166136261U;
static volatile uint32_t target_tx_failed;


volatile uint32_t uart_errors = 0;
volatile uint32_t queue_overflows = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* One ISR producer and one main-loop consumer per queue. */
static void queue_push(RxQueue *q, uint8_t value)
{
  uint16_t next = (uint16_t)((q->head + 1U) & 255U);
  if (next == q->tail) {
    queue_overflows++;
    return;
  }

  q->data[q->head] = value;
  __DMB();
  q->head = next;
}

static int queue_pop(RxQueue *q, uint8_t *value)
{
  if (q->head == q->tail) {
    return 0;
  }

  __DMB();
  *value = q->data[q->tail];
  __DMB();
  q->tail = (uint16_t)((q->tail + 1U) & 255U);
  return 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* No blocking UART transmission in the interrupt callback. */
  if (huart->Instance == USART1) {
    uint8_t received = target_byte;
    if (HAL_UART_Receive_IT(&huart1, &target_byte, 1U) != HAL_OK) {
      uart_errors++;
    }
    queue_push(&target_queue, received);
  }
  else if (huart->Instance == USART2) {
    uint8_t received = usb_byte;
    if (HAL_UART_Receive_IT(&huart2, &usb_byte, 1U) != HAL_OK) {
      uart_errors++;
    }
    queue_push(&usb_queue, received);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  uart_errors++;

  /* HAL can stop reception after an overrun. Re-arm if it was stopped;
   * non-blocking framing/parity errors can leave reception active. */
  if (huart->RxState == HAL_UART_STATE_READY) {
    if (huart->Instance == USART1) {
      if (HAL_UART_Receive_IT(&huart1, &target_byte, 1U) != HAL_OK) {
        uart_errors++;
      }
    }
    else if (huart->Instance == USART2 && LOCAL_TEST) {
      if (HAL_UART_Receive_IT(&huart2, &usb_byte, 1U) != HAL_OK) {
        uart_errors++;
      }
    }
  }
}

/* RM0394: USART1_TX uses DMA1 channel4, request2. Re-arm on DMA
 * completion while the last byte is still in the UART, avoiding inter-block
 * stop-and-wait gaps. RX capture and main-loop hashing stay independent. */
static void target_dma_tx_kick(void)
{
  if (target_tx_active || target_tx_failed || target_tx_head == target_tx_tail) return;
  uint32_t count = (target_tx_head + TARGET_DMA_TX_SIZE - target_tx_tail) % TARGET_DMA_TX_SIZE;
  if (count > TARGET_DMA_TX_SIZE - target_tx_tail) count = TARGET_DMA_TX_SIZE - target_tx_tail;
  if (count > 1024U) count = 1024U;
  DMA1_Channel4->CCR = 0U;
  DMA1->IFCR = DMA_IFCR_CGIF4;
  DMA1_Channel4->CMAR = (uint32_t)&target_dma_tx[target_tx_tail];
  DMA1_Channel4->CNDTR = count;
  target_tx_active = count;
  __DMB();
  DMA1_Channel4->CCR = DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_PL_1
                    | DMA_CCR_TCIE | DMA_CCR_TEIE | DMA_CCR_EN;
}

void DMA1_Channel4_IRQHandler(void)
{
  uint32_t flags = DMA1->ISR;
  if (flags & DMA_ISR_TEIF4) {
    DMA1_Channel4->CCR = 0U;
    DMA1->IFCR = DMA_IFCR_CGIF4;
    target_dma_tx_errors++;
    target_tx_failed = 1U;
    return;
  }
  if (flags & DMA_ISR_TCIF4) {
    DMA1_Channel4->CCR = 0U;
    DMA1->IFCR = DMA_IFCR_CGIF4;
    uint32_t count = target_tx_active;
    target_dma_transmitted_bytes += count;
    target_tx_tail = (target_tx_tail + count) % TARGET_DMA_TX_SIZE;
    target_tx_active = 0U;
    target_dma_tx_kick();
  }
}

/* Count full circular DMA laps. Modulo NDTR alone cannot detect lost laps. */
void DMA1_Channel5_IRQHandler(void)
{
  uint32_t flags = DMA1->ISR;
  if (flags & DMA_ISR_TEIF5) target_dma_errors++;
  if (flags & DMA_ISR_TCIF5) target_dma_rx_wraps++;
  DMA1->IFCR = DMA_IFCR_CGIF5;
}

/* USART1_RX: DMA1 channel 5, request 2 (RM0394 DMA1 request table).
 * Keep capture active while main transmits whole blocks back to the radio. */
static void target_dma_start(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
  DMA1_Channel5->CCR = 0U;
  DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~DMA_CSELR_C5S) | (2U << DMA_CSELR_C5S_Pos);
  DMA1_Channel5->CPAR = (uint32_t)&USART1->RDR;
  DMA1_Channel5->CMAR = (uint32_t)target_dma_rx;
  DMA1_Channel5->CNDTR = TARGET_DMA_RX_SIZE;
  DMA1->IFCR = DMA_IFCR_CGIF5;
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0U, 0U);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  DMA1_Channel5->CCR = DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_PL_1
                    | DMA_CCR_TCIE | DMA_CCR_TEIE | DMA_CCR_EN;
  USART1->ICR = USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_NCF | USART_ICR_PECF;
  DMA1_Channel4->CCR = 0U;
  DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~DMA_CSELR_C4S) | (2U << DMA_CSELR_C4S_Pos);
  DMA1_Channel4->CPAR = (uint32_t)&USART1->TDR;
  DMA1->IFCR = DMA_IFCR_CGIF4;
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1U, 0U);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  USART1->CR3 |= USART_CR3_DMAR | USART_CR3_DMAT;
}

static void target_dma_echo(void)
{
  uint32_t errors = USART1->ISR & (USART_ISR_ORE | USART_ISR_FE | USART_ISR_NE | USART_ISR_PE);
  if (errors != 0U) {
    uart_errors++;
    uart_last_error_flags = errors;
    if (errors & USART_ISR_ORE) uart_overrun_errors++;
    if (errors & USART_ISR_FE) uart_framing_errors++;
    if (errors & USART_ISR_NE) uart_noise_errors++;
    USART1->ICR = USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_NCF | USART_ICR_PECF;
  }
  /* Capture the lap count and NDTR atomically with respect to the ISR.
   * A pending TC means hardware wrapped before the ISR could count it.
   * At NDTR=0 the old lap's endpoint already represents that same byte. */
  uint32_t state = __get_PRIMASK(); __disable_irq();
  uint32_t wraps = target_dma_rx_wraps;
  uint32_t remaining = DMA1_Channel5->CNDTR;
  if (DMA1->ISR & DMA_ISR_TCIF5) {
    remaining = DMA1_Channel5->CNDTR;
    if (remaining) wraps++;
  }
  uint32_t produced = wraps * TARGET_DMA_RX_SIZE + TARGET_DMA_RX_SIZE - remaining;
  __set_PRIMASK(state);
  target_dma_produced_bytes = produced;
  uint32_t length = produced - target_dma_consumed;
  if (length > TARGET_DMA_RX_SIZE) {
    target_dma_overrun_bytes += length - TARGET_DMA_RX_SIZE;
    queue_overflows++;
    target_dma_consumed = produced - TARGET_DMA_RX_SIZE;
    length = TARGET_DMA_RX_SIZE;
  }
  target_dma_read = target_dma_consumed % TARGET_DMA_RX_SIZE;
  if (length > TARGET_DMA_RX_SIZE - target_dma_read) length = TARGET_DMA_RX_SIZE - target_dma_read;
  if (length > 128U) length = 128U;
  if (length > 0U) {
    uint32_t head = target_tx_head;
    uint32_t free_bytes = (target_tx_tail + TARGET_DMA_TX_SIZE - head - 1U) % TARGET_DMA_TX_SIZE;
    if (target_tx_failed) return;
    if (free_bytes < length) { target_dma_backpressure++; length = free_bytes; }
    if (!length) return;
    __DMB();
    for (uint32_t i = 0; i < length; ++i) {
      uint8_t value = target_dma_rx[target_dma_read + i];
      target_dma_tx[(head + i) % TARGET_DMA_TX_SIZE] = value;
      target_dma_rx_hash = (target_dma_rx_hash ^ value) * 16777619U;
      /* Hash accepted TX bytes here; keep completion IRQ short enough to
       * refill TDR while the previous character is still on the wire. */
      target_dma_tx_hash = (target_dma_tx_hash ^ value) * 16777619U;
    }
    target_dma_consumed += length;
    target_dma_read = target_dma_consumed % TARGET_DMA_RX_SIZE;
    target_dma_received_bytes += length;
    __DMB();
    uint32_t irq_state = __get_PRIMASK();
    __disable_irq();
    target_tx_head = (head + length) % TARGET_DMA_TX_SIZE;
    uint32_t pending = (target_tx_head + TARGET_DMA_TX_SIZE - target_tx_tail) % TARGET_DMA_TX_SIZE;
    if (pending > target_dma_tx_queue_peak) target_dma_tx_queue_peak = pending;
    target_dma_tx_kick();
    __set_PRIMASK(irq_state);
  }
}


/* Test-only control through ST-LINK USART2; target payload stays arbitrary. */
static uint32_t fixture_nominal_baud=921600U;
static uint32_t fixture_driver_baud=935064U;
static char fixture_command[48];
static unsigned fixture_command_used;
static uint32_t fixture_last_rx_count,fixture_last_rx_tick;
static int fixture_supported(uint32_t b)
{ return b==9600U || b==57600U || b==115200U || b==230400U || b==460800U || b==921600U; }
static void fixture_reply(const char *text)
{ (void)HAL_UART_Transmit(&huart2,(const uint8_t *)text,(uint16_t)strlen(text),100U); }
static void fixture_status(void)
{
  char reply[96];
  snprintf(reply,sizeof(reply),"OK %lu %lu %lu\n",(unsigned long)fixture_nominal_baud,
           (unsigned long)fixture_driver_baud,(unsigned long)USART1->BRR);
  fixture_reply(reply);
}
static void fixture_command_service(void)
{
  if(fixture_last_rx_count!=target_dma_received_bytes) {
    fixture_last_rx_count=target_dma_received_bytes;fixture_last_rx_tick=HAL_GetTick();
  }
  if(!(USART2->ISR & USART_ISR_RXNE)) return;
  char c=(char)USART2->RDR;
  if(c=='\r') return;
  if(c!='\n') {
    if(fixture_command_used<sizeof(fixture_command)-1) fixture_command[fixture_command_used++]=c;
    else { fixture_command_used=0;fixture_reply("ERR command-too-long\n"); }
    return;
  }
  fixture_command[fixture_command_used]=0;fixture_command_used=0;
  if(strcmp(fixture_command,"STATUS")==0) { fixture_status();return; }
  if(strncmp(fixture_command,"BAUD ",5)!=0) { fixture_reply("ERR command\n");return; }
  char *end;unsigned long rate=strtoul(fixture_command+5,&end,10);
  if(*end || !fixture_supported(rate)) { fixture_reply("ERR unsupported\n");return; }
  if(target_tx_failed || target_tx_active || target_tx_head!=target_tx_tail
     || target_dma_produced_bytes!=target_dma_consumed || !(USART1->ISR & USART_ISR_TC)
     || (USART1->ISR & USART_ISR_BUSY) || HAL_GetTick()-fixture_last_rx_tick<5U) {
    fixture_reply("ERR busy\n");return;
  }
  uint32_t driver=(uint32_t)(((uint64_t)rate*935064U+460800U)/921600U);
  uint32_t state=__get_PRIMASK();__disable_irq();
  DMA1_Channel5->CCR=0;DMA1_Channel4->CCR=0;
  USART1->CR3 &= ~(USART_CR3_DMAR | USART_CR3_DMAT);
  /* HAL timeout accounting needs SysTick while it waits for ACK flags. */
  __set_PRIMASK(state);
  huart1.Init.BaudRate=driver;
  if(HAL_UART_Init(&huart1)!=HAL_OK) {
    __set_PRIMASK(state);fixture_reply("ERR uart\n");Error_Handler();
  }
  __disable_irq();
  target_dma_read=target_dma_consumed=target_dma_rx_wraps=target_dma_produced_bytes=0;
  target_tx_head=target_tx_tail=target_tx_active=0;
  target_dma_start();target_uart_brr=USART1->BRR;
  fixture_nominal_baud=rate;fixture_driver_baud=driver;
  __set_PRIMASK(state);fixture_status();
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  if (LOCAL_TEST) {
    if (HAL_UART_Receive_IT(&huart1, &target_byte, 1U) != HAL_OK) Error_Handler();
  } else {
    target_dma_start();
  }

  if (LOCAL_TEST) {
    if (HAL_UART_Receive_IT(&huart2, &usb_byte, 1U) != HAL_OK) {
      Error_Handler();
    }

    static const uint8_t ready[] = "READY\r\n";
    if (HAL_UART_Transmit(&huart2, ready, sizeof(ready) - 1U, 100U) != HAL_OK) {
      Error_Handler();
    }
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint8_t value;
    if (!LOCAL_TEST) { target_dma_echo();fixture_command_service(); }

    /* Laptop USB/VCOM -> USART1 TX, during the local jumper test. */
    if (LOCAL_TEST && queue_pop(&usb_queue, &value)) {
      if (HAL_UART_Transmit(&huart1, &value, 1U, 100U) != HAL_OK) {
        uart_errors++;
      }
    }

    /* USART1 RX -> USB in local mode; USART1 RX -> TX in radio mode. */
    if (queue_pop(&target_queue, &value)) {
      UART_HandleTypeDef *destination = LOCAL_TEST ? &huart2 : &huart1;
      if (HAL_UART_Transmit(destination, &value, 1U, 100U) != HAL_OK) {
        uart_errors++;
      }
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 9;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  /* Fixture headroom:72MHz/77=935064 baud, +1.46% from nominal921600.
   * Radio B is928882 baud: relative mismatch0.67%, before HSI tolerance.
   * BRR78 left only0.16% nominal throughput margin, below HSI accuracy. */
  huart1.Init.BaudRate = 935064;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  target_uart_brr = USART1->BRR;

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD4_Pin */
  GPIO_InitStruct.Pin = LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD4_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
