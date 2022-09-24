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
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "sockets.h"
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
osThreadId ClientHandle;
osThreadId UsartHandle;
osThreadId Client_recvHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
extern uint32_t g_osRuntimeCounter ;
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{
	g_osRuntimeCounter = 0;
}

__weak unsigned long getRunTimeCounterValue(void)
{
return g_osRuntimeCounter;
}
/* USER CODE END 1 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

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
  /* definition and creation of Client */
  osThreadDef(Client, StartDefaultTask, osPriorityNormal, 0, 1024);
  ClientHandle = osThreadCreate(osThread(Client), NULL);

  /* definition and creation of Usart */
  osThreadDef(Usart, StartTask02, osPriorityIdle, 0, 128);
  UsartHandle = osThreadCreate(osThread(Usart), NULL);

  /* definition and creation of Client_recv */
  osThreadDef(Client_recv, StartTask03, osPriorityNormal, 0, 1024);
  Client_recvHandle = osThreadCreate(osThread(Client_recv), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
	
  struct sockaddr_in conn_addr;				//连接地址 
	int conn_fd;								        //服务器的 socked fd=file descriptor
	
  /* Infinite loop */
  for(;;)
  {	
		conn_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(conn_fd<0)
		{
		  vTaskDelay(10);
      continue;
		}
		conn_addr.sin_family = AF_INET;
		conn_addr.sin_port = htons(5001);
		conn_addr.sin_addr.s_addr = inet_addr("10.2.133.77");
		memset(&(conn_addr.sin_zero), 0, sizeof(conn_addr.sin_zero));    //?
		
		if( connect(conn_fd, (struct sockaddr *)&conn_addr, sizeof(struct sockaddr_in)) == -1)
		{
			printf("connect error\n");
			closesocket(conn_fd);
			vTaskDelay(10);
			continue;
		}
		printf("connect successful!conn_fd=%d\r\n",conn_fd);  
		while(1)
		{
			char bTmpSendBuf[]="hello server!This is F429ZIT6 connect!\n";
			 if(write(conn_fd,bTmpSendBuf,sizeof(bTmpSendBuf))<0)
        break;
			
			HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
			vTaskDelay(1000);
		}
		closesocket(conn_fd);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the Usart thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
		char *pbuffer = (char *)calloc(200, sizeof(char)); 
		vTaskList(pbuffer);
		printf("----------------------------------------------\r\n");
		printf("Task_name state         priority     remaining_stack  Task_Num\r\n");
		printf("%s", pbuffer);
		memset(pbuffer, 0, sizeof(char)*200);
		printf("----------------------------------------------\r\n");
		printf("Task_name   Running count                 usage\r\n");
    vTaskGetRunTimeStats(pbuffer);
    printf("%s", pbuffer);
    free(pbuffer);
		printf("----------------------------------------------\r\n");
		printf("FreeHeapSize:%d\n",xPortGetFreeHeapSize());
		
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		vTaskDelay(2000);
//    osDelay(1);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the Client_recv thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */

	char *recv_data;
	int recv_data_len;
	recv_data = (char *)pvPortMalloc(RECV_DATA);
  /* Infinite loop */
  for(;;)
  {
//			recv_data_len = recv(conn_fd, recv_data, RECV_DATA, 0);
//			if(recv_data_len>0)
//				write(conn_fd,recv_data,recv_data_len);
    osDelay(1);
  }
  /* USER CODE END StartTask03 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
