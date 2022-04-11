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

typedef struct qMessageSturct {
    int id;
    char data;
} qMesStruct;

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
TaskHandle_t  xsendTaskHandle1;
TaskHandle_t  xsendTaskHandle2;
TaskHandle_t  xrecTaskHandle;
TaskHandle_t  xwriteTaskHandle;
TaskHandle_t  xreadTaskHandle;
TaskHandle_t  xreadTaskHandle2;

void sendTask1(void * pvParameters);
void sendTask2(void * pvParameters);
void recTask(void * pvParameters);
void writeTask (void * pvParameters);
void readTask(void * pvParameters);
void readTask2(void * pvParameters);
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

  QueueHandle_t xqueueHandle1;
  xqueueHandle1 = xQueueCreate(5, sizeof(qMesStruct));

  QueueHandle_t xqueueHandle2;
  xqueueHandle2 = xQueueCreate(5, sizeof(qMesStruct));

  QueueHandle_t xMailboxHandle;
  xMailboxHandle = xQueueCreate(1, sizeof(qMesStruct)); // create a mailbox, note: mailbox just has a length of one

  QueueSetHandle_t xqueueSetHandle;
  xqueueSetHandle = xQueueCreateSet(10);

  BaseType_t xQHandle1AddStatus;
  BaseType_t xQHandle2AddStatus;
  xQHandle1AddStatus = xQueueAddToSet(xqueueHandle1, xqueueSetHandle);
  xQHandle2AddStatus = xQueueAddToSet(xqueueHandle2, xqueueSetHandle);

  if (xqueueHandle1 != NULL && xqueueHandle1 != NULL && xqueueSetHandle != NULL && xMailboxHandle != NULL && xQHandle1AddStatus != pdFAIL && xQHandle2AddStatus != pdFAIL) {
      xTaskCreate(sendTask1, "sendTask1", 512, (void *)xqueueHandle1, osPriorityNormal, &xsendTaskHandle1);
      xTaskCreate(sendTask2, "sendTask2", 512, (void *)xqueueHandle2, osPriorityNormal, &xsendTaskHandle2);
      xTaskCreate(recTask, "recTask", 512, (void *)xqueueSetHandle, osPriorityNormal1, &xrecTaskHandle);

      xTaskCreate(writeTask, "writeTask", 512, (void *)xMailboxHandle, osPriorityNormal, &xwriteTaskHandle);
      xTaskCreate(readTask, "readTask", 512, (void *)xMailboxHandle, osPriorityNormal1, &xreadTaskHandle);
      xTaskCreate(readTask2, "readTask2", 512, (void *)xMailboxHandle, osPriorityNormal1, &xreadTaskHandle2);
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

void sendTask1(void * pvParameters) {
    QueueHandle_t qSendHandle;
    qSendHandle = (QueueHandle_t)pvParameters;
    BaseType_t xQueueSendStatus;
    qMesStruct qSendUSB = {1, 11};
    while (1) {
        osDelay(2000);
        xQueueSendStatus = xQueueSend(qSendHandle, &qSendUSB, 10);
        if (xQueueSendStatus == pdPASS) {
            printf("Queue sendTask1 qSendUSB.id = %d qSendUSB.data = %d done \r\n", qSendUSB.id, qSendUSB.data);
            qSendUSB.data++;
        } else {
            printf("Queue send fail \r\n");
        }
    }
}


void sendTask2(void * pvParameters) {
    QueueHandle_t qSendHandle;
    qSendHandle = (QueueHandle_t)pvParameters;
    BaseType_t xQueueSendStatus;
    qMesStruct qSendUSB = {2, 22};
    while (1) {
        osDelay(2000);
        xQueueSendStatus = xQueueSend(qSendHandle, &qSendUSB, 10);
        if (xQueueSendStatus == pdPASS) {
            printf("Queue sendTask2 qSendUSB.id = %d qSendUSB.data = %d done \r\n", qSendUSB.id, qSendUSB.data);
            qSendUSB.data++;
        } else {
            printf("Queue send fail \r\n");
        }
    }
}

void recTask(void * pvParameters) {
    BaseType_t xQueueRecStatus;
    QueueSetHandle_t qSetRecHandle;
    qSetRecHandle = (QueueSetHandle_t)pvParameters;
    QueueSetMemberHandle_t qSetDataHandle;
    UBaseType_t uxNumberOfFreeSpaces;
    qMesStruct qRecUSB = {0, 0};
    while (1) {
        qSetDataHandle = xQueueSelectFromSet(qSetRecHandle, portMAX_DELAY);
        uxNumberOfFreeSpaces = uxQueueSpacesAvailable(qSetDataHandle);
        if (qSetDataHandle != NULL) {
            printf("Queue number of free spaces: %d \r\n", uxNumberOfFreeSpaces);
            xQueueRecStatus = xQueueReceive(qSetDataHandle, &qRecUSB, portMAX_DELAY);
            if (xQueueRecStatus == pdPASS) {
                printf("Queue receive qRecUSB.id = %d qRecUSB.data = %d done \r\n", qRecUSB.id, qRecUSB.data);
            } else {
                printf("Queue receive fail \r\n");
            }
        } else {
            printf("Queue set not become available \r\n");
        }
    }
}

void writeTask (void * pvParameters) {
    QueueHandle_t qWriteMailboxHandle;
    qWriteMailboxHandle = (QueueHandle_t)pvParameters;
    BaseType_t xWriteStatus;
    qMesStruct qMailboxData = {3, 3};
    while (1) {
        xWriteStatus = xQueueOverwrite(qWriteMailboxHandle, &qMailboxData);
        if(xWriteStatus != pdPASS) {
            printf("Write mail box fail \r\n");
        } else {
            printf("Write mail box data: %d done \r\n", qMailboxData.data);
        }
        qMailboxData.data++;
        osDelay(6000);
    }
}

void readTask(void * pvParameters) {
    QueueHandle_t qMailboxReadHandle;
    qMailboxReadHandle = (QueueHandle_t)pvParameters;
    BaseType_t xReadStatus;
    qMesStruct qMailboxData = {0, 0};
    while (1) {
        xReadStatus = xQueuePeek(qMailboxReadHandle, &qMailboxData, portMAX_DELAY);
        if (xReadStatus != pdPASS) {
            printf("Read mail box data fail \r\n");
        } else {
            printf("Read task1 mail box id %d; data: %d \r\n", qMailboxData.id, qMailboxData.data);
        }
        osDelay(3000);
    }
}

void readTask2(void * pvParameters) {
    QueueHandle_t qMailboxReadHandle;
    qMailboxReadHandle = (QueueHandle_t)pvParameters;
    BaseType_t xReadStatus;
    qMesStruct qMailboxData = {0, 0};
    while (1) {
        xReadStatus = xQueuePeek(qMailboxReadHandle, &qMailboxData, portMAX_DELAY);
        if (xReadStatus != pdPASS) {
            printf("Read task2 mail box data fail \r\n");
        } else {
            printf("Read task2 mail box id %d; data: %d \r\n", qMailboxData.id, qMailboxData.data);
        }
        osDelay(3000);
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

