/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "liquidcrystal_i2c.h"
#include <stdio.h> // Include stdio.h for sprintf
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
int pump1Count = 0;
int pump2Count = 0;
int pump3Count = 0;
int pump4Count = 0;
int totalPump = 5000;
int lcd_count = 0;
static bool pump1flag = false;
static bool pump2flag = false;
static bool pump3flag = false;
static bool pump4flag = false;
static bool OnPetrolflag = true;
char message[150];
uint32_t timer_start = 0;
uint32_t timer_end = 0;
uint32_t timer_needed = 0;
int p1 =0;
int p2 = 0;
int p3 = 0;
int p4 = 0;
int start = 0;
int end = 0;
int lcd = 0;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* Definitions for Task1 */
osThreadId_t Task1Handle;
const osThreadAttr_t Task1_attributes = {
  .name = "Task1",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task2 */
osThreadId_t Task2Handle;
const osThreadAttr_t Task2_attributes = {
  .name = "Task2",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task3 */
osThreadId_t Task3Handle;
const osThreadAttr_t Task3_attributes = {
  .name = "Task3",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task4 */
osThreadId_t Task4Handle;
const osThreadAttr_t Task4_attributes = {
  .name = "Task4",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for startLcd */
osThreadId_t startLcdHandle;
const osThreadAttr_t startLcd_attributes = {
  .name = "startLcd",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for pump1 */
osSemaphoreId_t pump1Handle;
const osSemaphoreAttr_t pump1_attributes = {
  .name = "pump1"
};
/* Definitions for pump2 */
osSemaphoreId_t pump2Handle;
const osSemaphoreAttr_t pump2_attributes = {
  .name = "pump2"
};
/* Definitions for pump3 */
osSemaphoreId_t pump3Handle;
const osSemaphoreAttr_t pump3_attributes = {
  .name = "pump3"
};
/* Definitions for pump4 */
osSemaphoreId_t pump4Handle;
const osSemaphoreAttr_t pump4_attributes = {
  .name = "pump4"
};
/* Definitions for startSwv */
osSemaphoreId_t startSwvHandle;
const osSemaphoreAttr_t startSwv_attributes = {
  .name = "startSwv"
};
/* USER CODE BEGIN PV */
osEventFlagsId_t PumpGroup;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
void pump1_thread(void *argument);
void pump2_thread(void *argument);
void pump3_thread(void *argument);
void pump4_thread(void *argument);
void start_Lcd(void *argument);

/* USER CODE BEGIN PFP */
void Task_action(char message);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if(GPIO_Pin == Pump1_Pin && OnPetrolflag){
		pump1flag = true;
	}

	else if(GPIO_Pin == Pump2_Pin && OnPetrolflag){
		pump2flag = true;
	}
	else if(GPIO_Pin == Pump3_Pin && OnPetrolflag){
		pump3flag = true;
	}
	else if(GPIO_Pin == Pump4_Pin && OnPetrolflag){
		pump4flag = true;
	}


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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */


  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of pump1 */
  pump1Handle = osSemaphoreNew(1, 1, &pump1_attributes);

  /* creation of pump2 */
  pump2Handle = osSemaphoreNew(1, 0, &pump2_attributes);

  /* creation of pump3 */
  pump3Handle = osSemaphoreNew(1, 0, &pump3_attributes);

  /* creation of pump4 */
  pump4Handle = osSemaphoreNew(1, 0, &pump4_attributes);

  /* creation of startSwv */
  startSwvHandle = osSemaphoreNew(1, 0, &startSwv_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task1 */
  Task1Handle = osThreadNew(pump1_thread, NULL, &Task1_attributes);

  /* creation of Task2 */
  Task2Handle = osThreadNew(pump2_thread, NULL, &Task2_attributes);

  /* creation of Task3 */
  Task3Handle = osThreadNew(pump3_thread, NULL, &Task3_attributes);

  /* creation of Task4 */
  Task4Handle = osThreadNew(pump4_thread, NULL, &Task4_attributes);

  /* creation of startLcd */
  startLcdHandle = osThreadNew(start_Lcd, NULL, &startLcd_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
//  const osEventFlagsAttr_t Flags_attr = {.name = "Triggering_PumpThread"};
//  PumpGroup = osEventFlagsNew(&Flags_attr);
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00303D5B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Stop1_GPIO_Port, Stop1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Stop2_GPIO_Port, Stop2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : Pump2_Pin */
  GPIO_InitStruct.Pin = Pump2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Pump2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Stop1_Pin */
  GPIO_InitStruct.Pin = Stop1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Stop1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Pump1_Pin */
  GPIO_InitStruct.Pin = Pump1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Pump1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Pump3_Pin Pump4_Pin */
  GPIO_InitStruct.Pin = Pump3_Pin|Pump4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Stop2_Pin */
  GPIO_InitStruct.Pin = Stop2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Stop2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void Task_action(char message)
{
	ITM_SendChar(message);
	ITM_SendChar('\n');
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_pump1_thread */
/**
  * @brief  Function implementing the Task1 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_pump1_thread */
void pump1_thread(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	 //osSemaphoreAcquire(pump1Handle, osWaitForever);
	 p1++;
	  timer_start = SysTick -> VAL;
	  if(totalPump>0 && pump1flag && OnPetrolflag ){
		  pump1flag = false;
		  totalPump --;
		  pump1Count ++;
		  timer_end = SysTick -> VAL;
	  }else if (totalPump == 0){

		  OnPetrolflag = false;
		  HAL_GPIO_WritePin(Stop1_GPIO_Port, Stop1_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(Stop2_GPIO_Port, Stop2_Pin, GPIO_PIN_SET);
	  }

	 //osSemaphoreRelease(pump2Handle);
	 // osThreadYield();
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_pump2_thread */
/**
* @brief Function implementing the Task2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pump2_thread */
void pump2_thread(void *argument)
{
  /* USER CODE BEGIN pump2_thread */

  /* Infinite loop */
  for(;;)
  {

	// osSemaphoreAcquire(pump2Handle, osWaitForever);
	 p2++;
	  if(totalPump>0 && pump2flag && OnPetrolflag ){
		  pump2flag = false;
		  totalPump --;
		  pump2Count ++;


	  }else if (totalPump == 0){
		  OnPetrolflag = false;
		  HAL_GPIO_WritePin(Stop1_GPIO_Port, Stop1_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(Stop2_GPIO_Port, Stop2_Pin, GPIO_PIN_SET);
	  }

	 //osSemaphoreRelease(pump3Handle);
	 // osThreadYield();
  }
  /* USER CODE END pump2_thread */
}

/* USER CODE BEGIN Header_pump3_thread */
/**
* @brief Function implementing the Task3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pump3_thread */
void pump3_thread(void *argument)
{
  /* USER CODE BEGIN pump3_thread */
  /* Infinite loop */
  for(;;)
  {
	  //osSemaphoreAcquire(pump3Handle, osWaitForever);
	  p3 ++;
	  if(totalPump>0 && pump3flag && OnPetrolflag ){
		  pump3flag = false;
		  totalPump --;
		  pump3Count ++;
	  }else if (totalPump == 0){
		  OnPetrolflag = false;
		  HAL_GPIO_WritePin(Stop1_GPIO_Port, Stop1_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(Stop2_GPIO_Port, Stop2_Pin, GPIO_PIN_SET);
	  }
	  //osSemaphoreRelease(pump4Handle);
	 // osThreadYield();
  }
  /* USER CODE END pump3_thread */
}

/* USER CODE BEGIN Header_pump4_thread */
/**
* @brief Function implementing the Task4 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pump4_thread */
void pump4_thread(void *argument)
{
  /* USER CODE BEGIN pump4_thread */

  /* Infinite loop */
  for(;;)
  {
	  //osSemaphoreAcquire(pump4Handle, osWaitForever);
	  p4++;
	  if(totalPump>0 && pump4flag && OnPetrolflag ){
		  pump4flag = false;
		  totalPump --;
		  pump4Count ++;

	  }else if (totalPump == 0){
		  OnPetrolflag  = false;
		  HAL_GPIO_WritePin(Stop1_GPIO_Port, Stop1_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(Stop2_GPIO_Port, Stop2_Pin, GPIO_PIN_SET);
	  }
	  //osSemaphoreRelease(pump1Handle);
	 // osThreadYield();
  }
  /* USER CODE END pump4_thread */
}

/* USER CODE BEGIN Header_start_Lcd */
/**
* @brief Function implementing the startLcd thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_Lcd */
void start_Lcd(void *argument)
{
  /* USER CODE BEGIN start_Lcd */
	TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialize the xLastWakeTime variable with the current time.
	const TickType_t xFrequency = 100; // 50 ticks = 50ms if configTICK_RATE_HZ is 1000.
	char message[100];
  /* Infinite loop */
  for(;;)
  {
	  //osSemaphoreAcquire(startLcdHandle, osWaitForever);
	  vTaskDelayUntil(&xLastWakeTime, xFrequency); // this thread will run every 100ms , 100uwTick
	  lcd++;
	  if(totalPump !=50000){
	      unsigned int x= sprintf(message, "Total Pump: %d, Lcd count: %d, xTickCount: %d \r\n",
					  totalPump, lcd_count , xTaskGetTickCount());

		  // Send each character of the message using ITM_SendChar

		  for (int i = 0; i<x; ++i) {
			  ITM_SendChar(message[i]);
		  }
	  }
//	  Task_action(totalPump);
	 // osThreadYield();

	  //osSemaphoreRelease(pump1Handle);

  }
  /* USER CODE END start_Lcd */
}

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
