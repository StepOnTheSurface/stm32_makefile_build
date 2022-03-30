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
#include "queue.h"
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
  .stack_size = 512 * 4,
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
TaskHandle_t  xsendTaskHandle;
TaskHandle_t  xrecTaskHandle;
QueueHandle_t xqueueHandle;
void sendTask(void * pvParameters);
void recTask(void * pvParameters);
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

  xqueueHandle = xQueueCreate(5, sizeof(int));
  if (xqueueHandle != NULL) {
      xTaskCreate(sendTask, "sendTask", 512, (void *)&xqueueHandle, osPriorityNormal, &xsendTaskHandle);
      xTaskCreate(recTask, "recTask", 512, (void *)&xqueueHandle, osPriorityNormal, &xrecTaskHandle);
      printf("Create queue successfully \r\n");
  } else {
      printf("Create queue failed \r\n");
  }
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
      osDelay(1000);
      TickType_t tickCountStart = xTaskGetTickCount();
      printf("Current tick count: %ld \r\n", tickCountStart);

      BaseType_t shedulerState = xTaskGetSchedulerState();
      switch (shedulerState) {
          case taskSCHEDULER_SUSPENDED:
              printf("Sheduler state: suspended \r\n");
              break;

          case taskSCHEDULER_NOT_STARTED:
              printf("Sheduler state: not started \r\n");
              break;

          case taskSCHEDULER_RUNNING:
              printf("Sheduler state: running \r\n");
              break;

          default:
            break;
      }

      UBaseType_t ledBlinkyHandle1WaterMark = uxTaskGetStackHighWaterMark(xLedBlinkyHandle1);
      printf("ledBlinkyHandle1 high water mark: %ld \r\n", ledBlinkyHandle1WaterMark);
      UBaseType_t StartDefaultTaskWaterMark = uxTaskGetStackHighWaterMark(NULL);
      printf("StartDefaultTask high water mark: %ld \r\n", StartDefaultTaskWaterMark);

      const char *name = "";
      name = pcTaskGetName(xLedBlinkyHandle1);
      printf("xLedBlinkyHandle1 task get name: %s \r\n", name); // task name string does not more than configMAX_TASK_NAME_LEN
      name = pcTaskGetName(NULL);
      printf("Current task get name: %s \r\n", name);

      TickType_t tickCountEnd = xTaskGetTickCount();
      printf("Current tick count: %ld \r\n", tickCountEnd);
      printf("Running default task spend time: %ld ms \r\n", tickCountEnd - tickCountStart);

      long lReturnedTaskHandle;
      lReturnedTaskHandle = ( long ) xTaskGetApplicationTaskTag(NULL);
      printf("Current task tag: %ld \r\n", lReturnedTaskHandle);
      if (lReturnedTaskHandle == NULL) {
          vTaskSetApplicationTaskTag(NULL, (void *) 1);
      }

      eTaskState ledBlinkyTask2State = eNoAction;
      ledBlinkyTask2State = eTaskGetState(xLedBlinkyHandle2);
      printf("xLedBlinkyHandle2 state: %d \r\n", ledBlinkyTask2State);

      UBaseType_t taskNum = uxTaskGetNumberOfTasks();
      printf("Current task num: %ld \r\n", taskNum);

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

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void sendTask(void * pvParameters) {
    BaseType_t xQueueSendStatus;
    int i = 0;
    while (1) {
        osDelay(1000);
        xQueueSendStatus = xQueueSend(xqueueHandle, &i, 10);
        if (xQueueSendStatus == pdPASS) {
            printf("Queue send i = %d done \r\n", i);
        } else {
            printf("Queue send fail \r\n");
        }
        i++;
    }
}

void recTask(void * pvParameters) {
    BaseType_t xQueueRecStatus;
    int j = 0;
    while (1) {
        osDelay(1000);
        xQueueRecStatus = xQueueReceive(xqueueHandle, &j, 10);
        if (xQueueRecStatus == pdPASS) {
            printf("Queue receive j = %d done \r\n", j);
        } else {
            printf("Queue receive fail \r\n");
        }
    }
}

void ledBlinkyTask1(void * pvParameters) {
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  while (1) {
      osDelay(1000);
      printf("--------------------\r\n");
      printf("Task1 wait notification \r\n");
      // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      uint32_t ulNotifiedValue;
      xTaskNotifyWait(0x00, /* Don't clear any notification bits on entry. */
                      0xffffffffUL, /* Reset the notification value to 0 on exit. */
                      &ulNotifiedValue, /* Notified value pass out in ulNotifiedValue. */
                      portMAX_DELAY); /* Block indefinitely. */
      /* Process any events that have been latched in the notified value. */
      if( ( ulNotifiedValue & 0x01 ) != 0 ) {
          /* Bit 0 was set - process whichever event is represented by bit 0. */
          printf("Bit 0 was set \r\n");
      } else if( ( ulNotifiedValue & 0x02 ) != 0 ) {
          /* Bit 1 was set - process whichever event is represented by bit 1. */
          printf("Bit 1 was set \r\n");
      } else if( ( ulNotifiedValue & 0x04 ) != 0 ) {
          /* Bit 2 was set - process whichever event is represented by bit 2. */
          printf("Bit 2 was set \r\n");
      }
      HAL_GPIO_TogglePin(GPIOG, LED3_Pin); // Toggle LED3
  }
  /* USER CODE END StartDefaultTask */
}

void ledBlinkyTask2(void * pvParameters) {
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  while (1) {
      osDelay(2000);
	    HAL_GPIO_TogglePin(GPIOG, LED4_Pin); // Toggle LED4
      printf("--------------------\r\n");
      printf("Task2 notify taks1 \r\n");
      // xTaskNotifyGive(xLedBlinkyHandle1);
      // if (xTaskNotify(xLedBlinkyHandle1, 0x02, eSetValueWithOverwrite) == pdPASS) {
      if (xTaskNotifyAndQuery( xLedBlinkyHandle1, ( 1UL << 2UL ), eSetBits, NULL ) == pdPASS) {
          printf("Task2 notify taks1 value was updated \r\n");
      } else {
          printf("Task2 notify taks1 value was not updated \r\n");
      }
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE END Application */

