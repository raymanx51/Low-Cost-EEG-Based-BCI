/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>

#include "utils.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


TIM_HandleTypeDef htim3;




/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */




// We are defining a type of thing. We will have many of these within an array
// Old Test Code
// #define ADC_BUF_LEN 4096


/* Enable Printing of Receive Messages From USART */
#define RECEIVE_PRINT_EN		0
#define ADC_BUFF_SIZE			512
#define ADC_BUFF_HALF_SIZE 		ADC_BUFF_SIZE/2
/* The offset is the size of adc array as this accounts for casting 16bit to 8bit int */
#define ADC_BUFF_OFFSET			ADC_BUFF_SIZE
#define UART_RX_BUFFER_SIZE		256
#define END_OF_MESSAGE_CHAR		'\n'
#define START_STR			"start\n"











/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */








/* __IO is volatile in HAL library */
__IO uint8_t *print_str_ptr = NULL;
__IO uint16_t adc_val[ADC_BUFF_SIZE] = { 0 }; /* Where the ADC values are stored */
__IO uint16_t adc_array_offset = 0; /* Offset for sending first or second half of ADC buffer */
__IO uint8_t adc_print = 0; /* Print adc values flag */
__IO uint8_t str_print = 0; /* Print string flag */
__IO uint8_t end_of_message = 0; /* Flag is set when start of string has be sent */
__IO uint8_t start = 0; /* Flag to start main loop after receiving response from PC */

uint8_t rx_buffer[UART_RX_BUFFER_SIZE] = { 0 }; /* Buffer for received messages over USART */
circle_buff_s rx_control = { /* Struct for managing receive buffer */.buffer = rx_buffer, .head = 0, .tail = 0, .size = UART_RX_BUFFER_SIZE,
		.wrapped = 0 };








// Old Test Code
// We use many of those buffers and keep em in this array
//uint16_t adc_buf[ADC_BUF_LEN];








/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  // OLD TEST CODE
  // HAL library will start the DMC for us
  // HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, ADC_BUF_LEN);





	/* Enable interrupt on single Byte receive in USART buffer */
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);

	/* Workaround for asynchronous UART RX */
	huart1.pRxBuffPtr = rx_control.buffer; /* Tell USART where to place received bytes */
	huart1.Mask = 0xFFU; /* Setting no parity bits */











  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
/* Loop around until "start\n" is received over USART */
  while (!start)
  {

	  HAL_printf("s"); /* Send "ss/n" over USART */
	  HAL_Delay(500); /* Delay 500ms */
  }

  /* Received start from PC, acknowledge and start */
  	HAL_printf("starting");

  	/* Calibrate ADC */
  	HAL_Status_Check(HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED));

  	/* Start DMA controller taking samples from ADC and placing them in buffer */
  	HAL_Status_Check(
  			HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &adc_val, ADC_BUFF_SIZE));

  	/* Start timer used to trigger ADC conversions, timer set to 1KHz */
  	HAL_Status_Check(HAL_TIM_Base_Start_IT(&htim3));

  	/* Main Loop, sends the half the ADC buffer every 256ms and can send received messaged if enabled */





    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */







  	while (1) {

		/* ADC values ready to send */
				if (adc_print) {
					/* This is actually sending half of the ADC buffer because of the cast, 512 uin8_t's or 256 uint16_t's */
					HAL_print_raw((uint8_t*) (&adc_val) + adc_array_offset,
					ADC_BUFF_SIZE);
					adc_print = 0;
				}
				/* Received message over USART, print it back */
				if (str_print && RECEIVE_PRINT_EN) {
					/* Full message received, send in one go */
					if (!end_of_message) {
						HAL_printf("Received %s", print_str_ptr);
					}
					/* Start of message already sent, print end of message */
					else {
						HAL_print_raw((uint8_t*) print_str_ptr, rx_control.head);
						HAL_print_raw((uint8_t*) '\n', 1);
						end_of_message = 0;
					}
					str_print = 0;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */
  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 100 - 1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 640;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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
  huart1.Init.BaudRate = 57600;
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
  huart2.Init.BaudRate = 38400;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */





// Called When the ADC has filled half of the buffer
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
	//* Set flag to send first half of buffer *
	adc_print = 1;
	adc_array_offset = 0;
}


// Called when the ADC has filled the buffer
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	// Set flag to send second half of buffer
	adc_print = 1;
	adc_array_offset = ADC_BUFF_OFFSET;
 	 // Tell DMA controller to restart filling the buffer
	HAL_Status_Check(
			HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &adc_val, ADC_BUFF_SIZE));
}


/* Called when the UART has processed 1 byte, had to implement my own circular buffer this way
 * as HAL library didn't facilitate asynchronous receiving
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	/* End of buffer, print start of message */
	if ((uint32_t) huart->pRxBuffPtr
			>= ((uint32_t) (rx_control.buffer) + rx_control.size - 1)) {
		print_str_ptr = rx_control.buffer + rx_control.tail;
		huart->pRxBuffPtr = rx_control.buffer;
		rx_control.head = 0;
		rx_control.tail = 0;
		rx_control.wrapped = 1;
		str_print = 1;
	} else {
		rx_control.head++;
	}

	/* Full message received, process it */
	if ((uint8_t) *(rx_control.buffer + rx_control.head - 1)
			== (uint8_t) END_OF_MESSAGE_CHAR) {
		/* Don't set print_str_ptr if it was set already above */
		if (!str_print) {
			print_str_ptr = rx_control.buffer + rx_control.tail;
			if (RECEIVE_PRINT_EN) {
				str_print = 1;
			}
		}

		/* Start message received from PC, start main loop */
		if (!start
				&& !(strncmp((char*) print_str_ptr, START_STR,
						strlen(START_STR)))) {
			start = 1;
		}
		rx_control.tail = rx_control.head;

		/* If receive buffer has wrapped around, print end of message and memset rest of buffer */
		if (rx_control.wrapped) {
			rx_control.wrapped = 0;
			/* memset everything after current message */
			memset(rx_control.buffer + rx_control.tail, 0,
					rx_control.size - rx_control.tail);
			end_of_message = 1;
		}
	}

	/* Re-enable byte received interrupt */
	__HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
}










/*
// Called when first half of buffer is filled
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc){
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}



// Called when buffer is completely full
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}
*/







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

#ifdef  USE_FULL_ASSERT
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
