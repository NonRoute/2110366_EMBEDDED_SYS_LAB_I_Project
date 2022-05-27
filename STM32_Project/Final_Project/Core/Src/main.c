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
#include <math.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define TIMCLOCK 84000000
#define PRESCALAR 84

//value for sensor calibration
#define RED_a 97.859
#define RED_b -770.34
#define GREEN_a 89.49
#define GREEN_b -666.01
#define BLUE_a 102.26
#define BLUE_b -823.62

enum Scaling { //output frequency scaling of color sensor
	Scl0, Scl2, Scl20, Scl100
};

enum Filter { //filter of color sensor
	Red, Blue, Clear, Green
};

uint8_t set_color; //Red, BLue, Green
int wait_for_callback = 0; //0 when receive callback, 1 when waiting for callback
double frequency = 0; //update when get interrupt callback
double Output_Color = 0; //update when get interrupt callback

double RGB[3]; //Array [Red, Green, Blue]

/* Measure Frequency */
uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
int Is_First_Captured = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
		if (Is_First_Captured == 0) // if the first rising edge is not captured
				{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
		}

		else // If the first rising edge is captured, now we will capture the second edge
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3); // read second value

			if (IC_Val2 > IC_Val1) { //Avoid overflow
				Difference = IC_Val2 - IC_Val1;
			}

			else if (IC_Val1 > IC_Val2) {
				Difference = (0xffffffff - IC_Val1) + IC_Val2;
			}

			float refClock = TIMCLOCK / (PRESCALAR);

			frequency = refClock / Difference;

			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
			Is_First_Captured = 0; // set it back to false

			//Frequency to Color -> Depending of the filter
			switch (set_color) {
			case Red:
				Output_Color = RED_a*log(frequency) + RED_b;
				break;

			case Green:
				Output_Color = GREEN_a*log(frequency) + GREEN_b;
				break;

			case Blue:
				Output_Color = BLUE_a*log(frequency) + BLUE_b;
				break;
			}

			//Constrain Value to MaxRange
			if (Output_Color > 255)
				Output_Color = 255;
			if (Output_Color < 0)
				Output_Color = 0;

			wait_for_callback = 0;
		}
	}
}

void Set_Scaling(int mode) {
	switch (mode) {
	case (Scl0): //OUTPUT FREQUENCY SCALING = 0%
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0); //S0 L
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0); //S1 L
		break;
	case (Scl2): //2%
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0); //S0 L
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1); //S1 H
		break;
	case (Scl20): //20%
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1); //S0 H
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0); //S1 L
		break;
	case (Scl100): //100%
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1); //S0 H
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1); //S1 H
		break;
	}
}

void Set_Filter(uint8_t mode) //Mode is type enum Filter
{
	set_color = mode;
	switch (mode) {
	case (Red):
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0); //S2 L
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0); //S3 L
		break;
	case (Blue):
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0); //S2 L
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1); //S3 H
		break;
	case (Clear):
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1); //S2 H
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0); //S3 L
		break;
	case (Green):
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1); //S2 H
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1); //S3 H
		break;
	}
}

void Print_Output() { //send RGB value by UART1 to NodeMCU
	char buffer[100];
	sprintf(buffer, "%d %d %d\r\n", (int) round(RGB[0]), (int) round(RGB[1]),
			(int) round(RGB[2]));
	HAL_UART_Transmit(&huart1, &buffer, strlen(buffer), HAL_MAX_DELAY);
}

void Print_Frequency(uint8_t set_color, float sum_frequency) { //send RGB and frequency by UART (used for sensor calibration)
	char buffer[100];
	switch (set_color) {
	case Red:
		sprintf(buffer, "Red = %d frequency = %d \r\n", (int) round(RGB[0]),
				(int) round(sum_frequency));
		break;
	case Green:
		sprintf(buffer, "Green = %d frequency = %d \r\n", (int) round(RGB[1]),
				(int) round(sum_frequency));
		break;
	case Blue:
		sprintf(buffer, "Blue = %d frequency = %d \r\n", (int) round(RGB[2]),
				(int) round(sum_frequency));
		break;
	}
	HAL_UART_Transmit(&huart2, &buffer, strlen(buffer), HAL_MAX_DELAY);
}

float GetColor(uint8_t set_color) { //set filter and return color value get from interrupt callback function
	Set_Filter(set_color);
	wait_for_callback = 1;
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3); //start interrupt
	while (wait_for_callback == 1) { //wait until value is get on the interrupt routine
	}
	HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_3); //stop interrupt
	return Output_Color;
}

void ReadColor(int read_times) { //read color value for 'read_times' times and calculate average value
	RGB[0] = 0;
	for (int i = 0; i < read_times; i++) {
		RGB[0] += GetColor(Red);
	}
	RGB[0] /= read_times;

	RGB[1] = 0;
	for (int i = 0; i < read_times; i++) {
		RGB[1] += GetColor(Green);
	}
	RGB[1] /= read_times;

	RGB[2] = 0;
	for (int i = 0; i < read_times; i++) {
		RGB[2] += GetColor(Blue);
	}
	RGB[2] /= read_times;
}

void ReadColorWithFrequency(int read_times, int delay) { //for sensor calibration
	float sum_frequency; //for calculate average frequency (only used for sensor calibration)
	while (1) {
		char buffer[100];
		sprintf(buffer,
				"-------------------Sensor Calibration-----------------\r\n");
		HAL_UART_Transmit(&huart2, &buffer, strlen(buffer), HAL_MAX_DELAY);

		RGB[0] = 0;
		sum_frequency = 0;
		for (int i = 0; i < read_times; i++) {
			RGB[0] += GetColor(Red);
			sum_frequency += frequency;
		}
		RGB[0] /= read_times;
		sum_frequency /= read_times;
		Print_Frequency(Red, sum_frequency);

		RGB[1] = 0;
		sum_frequency = 0;
		for (int i = 0; i < read_times; i++) {
			RGB[1] += GetColor(Green);
			sum_frequency += frequency;
		}
		RGB[1] /= read_times;
		sum_frequency /= read_times;
		Print_Frequency(Green, sum_frequency);

		RGB[2] = 0;
		sum_frequency = 0;
		for (int i = 0; i < read_times; i++) {
			RGB[2] += GetColor(Blue);
			sum_frequency += frequency;
		}
		RGB[2] /= read_times;
		sum_frequency /= read_times;
		Print_Frequency(Blue, sum_frequency);

		HAL_Delay(delay);
	}
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
	MX_TIM3_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */

	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);

	Set_Scaling(Scl20); //set color sensor's output frequency scaling to 20%

	int undetected_time = 0;
	const int detected_delay = 20;
	int is_detected = 0;
	uint32_t lastClap = HAL_GetTick();
	int clapCount = 0;
	const int timeBetweenClap = 1000;

//	ReadColorWithFrequency(1000, 3000); //uncomment for sensor calibration

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == HAL_OK) { //detected sound
			if (!is_detected && undetected_time > detected_delay) { //undetected sound before and undetected more than detected_delay
				if (HAL_GetTick() <= lastClap + timeBetweenClap)
					clapCount += 1;
				else
					clapCount = 1; //time between claps too long, reset clapCount to 1
				if (clapCount >= 3) {

					HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); //toggle LED
					ReadColor(100); //read color
					Print_Output(); //send color's value through UART1 to NodeMCU

					clapCount = 0; //reset clapCount
				}
				lastClap = HAL_GetTick();
			}
			is_detected = 1;
			undetected_time = 0;
		} else { // undetected sound
			undetected_time++;
			is_detected = 0;
		}
		HAL_Delay(1); //delay 1 ms
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_IC_InitTypeDef sConfigIC = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 84 - 1;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 20000 - 1;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_IC_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 5;
	if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_3) != HAL_OK) {
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
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
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
static void MX_USART2_UART_Init(void) {

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
	if (HAL_UART_Init(&huart2) != HAL_OK) {
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
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4,
			GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : PC0 */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PB1 PB2 PB3 PB4 */
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
