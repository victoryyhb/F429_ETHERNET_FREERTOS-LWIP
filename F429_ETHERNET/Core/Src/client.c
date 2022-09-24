/**
  ******************************************************************************
  * File Name          : client.c
  * Description        : serve as client
  ******************************************************************************
  * @attention
  *
  *
  * @author:yhb
  *
  *
  ******************************************************************************
  */
	
#include "client.h"
#include "cmsis_os.h"
#include "sockets.h"
#include "task.h"

osThreadId Client_threadHandle;
osThreadId TCP_Send_threadHandle;
osThreadId TCP_Recv_threadHandle;

void TCP_Send(void const * conn_fd)  //?参数为什么这么写
{
	int conn_fd_t = *(int*)conn_fd;
	for(;;)
  {
		char bTmpSendBuf[]="hello server!This is F429ZIT6 connect!\n";
	  if(write(conn_fd_t,bTmpSendBuf,sizeof(bTmpSendBuf))>0)                        //还未考虑TCP断开连接发送不成功的情况   //write的返回值可能是发了多少数据
		{
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			vTaskDelay(1000);
		}
		else
			vTaskDelay(1000);
  }
}

void TCP_Recv(void const * conn_fd)
{
	int conn_fd_t = *(int*)conn_fd;
	char *recv_data;
	int recv_data_len;
	
	recv_data = (char *)pvPortMalloc(RECV_DATA);
	for(;;)
  {
		recv_data_len = recv(conn_fd_t, recv_data, RECV_DATA, 0);                  //还未考虑TCP断开连接发送不成功和未接收到数据情况
		if(recv_data_len>0)
		{
			write(conn_fd_t,recv_data,recv_data_len);
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		}
		
    osDelay(1);
  }
}



//进行TCP连接，并创建TCP收发两个线程
void Client(void const * argument)
{
	struct sockaddr_in conn_addr;				//连接地址 
	int conn_fd;								        //服务器的 socked fd=file descriptor
	
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
		
	  osThreadDef(TCP_Send_thread, TCP_Send, osPriorityNormal, 0, 1024);
    TCP_Send_threadHandle = osThreadCreate(osThread(TCP_Send_thread),&conn_fd);
		
		osThreadDef(TCP_Recv_thread, TCP_Recv, osPriorityNormal, 0, 1024);
    TCP_Recv_threadHandle = osThreadCreate(osThread(TCP_Recv_thread), &conn_fd);
		
		vTaskDelete(Client_threadHandle);
		
    osDelay(1);
  }
}



//创建一个任务来进行TCP连接
void ClientInit()
{
	osThreadDef(Client_thread, Client, osPriorityNormal, 0, 1024);
  Client_threadHandle = osThreadCreate(osThread(Client_thread), NULL);
}


