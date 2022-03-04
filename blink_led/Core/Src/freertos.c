/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

typedef struct testTaskStruct {
      int testNum;
      char* testChar;
  } xTestTaskStruct;

xTestTaskStruct xTestStruct = {
    10,
    "TestTask"
  };

TaskHandle_t xLedBlinkyHandle1;
TaskHandle_t xLedBlinkyHandle2;
void ledBlinkyTask1(void * pvParameters);
void ledBlinkyTask2(void * pvParameters);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  xTaskCreate(ledBlinkyTask1, "ledBlinkyTask1", 1024, (void *)&xTestStruct, osPriorityNormal, &xLedBlinkyHandle1);
  xTaskCreate(ledBlinkyTask2, "ledBlinkyTask2", 1024, (void *)&xTestStruct, osPriorityNormal1, &xLedBlinkyHandle2);

  const char *pcNameToQuery = "ledBlinkyTask1";
  TaskHandle_t xHandle;
  xHandle =  xTaskGetHandle(pcNameToQuery);
  if (xHandle != NULL) {
      printf("There is a task named: %s \r\n", pcNameToQuery);
  } else {
      printf("There is not a task named: %s \r\n", pcNameToQuery);
  }
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void ledBlinkyTask1(void * pvParameters) {
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  int count = 0;
  eTaskState ledBlinkyTask2State = eNoAction;
  while (1) {
      osDelay(1000);
      HAL_GPIO_TogglePin(GPIOG, LED3_Pin); // Toggle LED3
      count++;
      if(10 == count) {
          ledBlinkyTask2State = eTaskGetState(xLedBlinkyHandle2);
          if (ledBlinkyTask2State == eSuspended) {
              vTaskResume(xLedBlinkyHandle2);
          }
      } else if (15 == count) {
          vTaskSuspendAll();
          // Noted: Other FreeRTOS API functions should not be called while the scheduler is suspended.
          for (int i = 0; i < 5; i++) {
              HAL_Delay(1000);
              HAL_GPIO_TogglePin(GPIOG, LED3_Pin); // Toggle LED3
          }
          xTaskResumeAll();
      }
      printf("xLedBlinkyHandle2 state: %d \r\n", ledBlinkyTask2State);

      UBaseType_t taskNum = uxTaskGetNumberOfTasks();
      printf("Current task num: %ld \r\n", taskNum);
      if (5 == taskNum) { // StartDefaultTask, ledBlinkyTask1, ledBlinkyTask2...
          for (int i = 0; i < 6; i++) {
              HAL_Delay(300);
              HAL_GPIO_TogglePin(GPIOG, LED3_Pin); // Toggle LED3
          }
      }

      static char cBuffer[512] = {0};
      vTaskList(cBuffer);
      printf("---------------------------------------------\r\n");
      printf("Name      State   Priority    Stack    Num\r\n");
      printf("*********************************************\r\n");
      printf("%s", cBuffer);
      printf("*********************************************\r\n");

  }
  /* USER CODE END StartDefaultTask */
}

void ledBlinkyTask2(void * pvParameters) {
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  int count = 0;
  while (1) {
      osDelay(1000);
	    HAL_GPIO_TogglePin(GPIOG, LED4_Pin); // Toggle LED4
      count++;
      if (6 == count) {
          vTaskSuspend(NULL);
      }
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE END Application */

