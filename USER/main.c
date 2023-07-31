#include "sys.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "delay.h"
#include "event_groups.h"
#include "key.h"
#include "oled_iic.h"
#include "rc522.h"
#include "as608.h"
#include "esp8266.h"
#include "usart2.h"
#include "usart3.h"
#include "beep.h"
#include "string.h"


volatile int err=0;
int RFID_Flag,AS608_Flag,ESP8266_Flag;
EventGroupHandle_t xEventHandle;
#define EventBit0 (1<<0)
#define EventBit1 (1<<1)
#define EventBit2 (1<<2)
#define EventBitAll (EventBit0 | EventBit1 | EventBit2)
//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_TASK_SIZE 		512  
//任务句柄
TaskHandle_t xStartTaskHandler;
//任务函数
void Start_Task(void *pvParameters);

//任务优先级
#define LCD_TASK_PRIO		3
//任务堆栈大小	
#define LCD_TASK_SIZE 		512
//任务句柄
TaskHandle_t xLcdTaskHandle;
//任务函数
void LCD_Task(void *pvParameters);

//任务优先级
#define RFID_TASK_PRIO		3
//任务堆栈大小	
#define RFID_TASK_SIZE 		512  
//任务句柄
TaskHandle_t xRFIDTaskHandle;
//任务函数
void RFID_Task(void *pvParameters);

//任务优先级
#define AS608_TASK_PRIO		3
//任务堆栈大小	
#define AS608_TASK_SIZE 		512  
//任务句柄
TaskHandle_t xAS608TaskHandle;
//任务函数
void AS608_Task(void *pvParameters);

//任务优先级
#define ESP8266_TASK_PRIO		3
//任务堆栈大小	
#define ESP8266_TASK_SIZE 		512
//任务句柄
TaskHandle_t xESP8266TaskHandle;
//任务函数
void ESP8266_Task(void *pvParameters);

//任务优先级

#define Admin_TASK_PRIO		5
//任务堆栈大小	
#define Admin_TASK_SIZE 		512
//任务句柄
TaskHandle_t xAdminTaskHandle;
//任务函数
void Admin_Task(void *pvParameters);

//任务优先级
#define LED_TASK_PRIO		4
//任务堆栈大小	
#define LED_TASK_SIZE 		512  
//任务句柄
TaskHandle_t xLEDTaskHandle;
//任务函数
void LED_Task(void *pvParameters);




void Start_Task(void *pvParameters)
{
	BaseType_t xReturn;
	taskENTER_CRITICAL();           //停止中断与任务器调度
	
	xEventHandle = xEventGroupCreate();
	if(xEventHandle != NULL)
	{
		printf("事件组创建成功\n");
	}
	
	xReturn = xTaskCreate(LCD_Task,"LCD_Task",LCD_TASK_SIZE,NULL,LCD_TASK_PRIO,&xLcdTaskHandle);
	if(xReturn == pdPASS)
	{
		printf("LCD任务创建成功\n");
	}
	
	xReturn = xTaskCreate(RFID_Task,"RFID_Task",RFID_TASK_SIZE,NULL,RFID_TASK_PRIO,&xRFIDTaskHandle);
	if(xReturn == pdPASS)
	{
		printf("RFID任务创建成功\n");
	}
	
	xReturn = xTaskCreate(AS608_Task,"AS608_Task",AS608_TASK_SIZE,NULL,AS608_TASK_PRIO,&xAS608TaskHandle);
	if(xReturn == pdPASS)
	{
		printf("AS608任务创建成功\n");
	}
	
	xReturn = xTaskCreate(ESP8266_Task,"ESP8266_Task",ESP8266_TASK_SIZE,NULL,ESP8266_TASK_PRIO,&xESP8266TaskHandle);
	if(xReturn == pdPASS)
	{
		printf("ESP8266任务创建成功\n");
	}
	
	xReturn = xTaskCreate(LED_Task,"LED_Task",LED_TASK_SIZE,NULL,LED_TASK_PRIO,&xLEDTaskHandle);
	if(xReturn == pdPASS)
	{
		printf("LED任务创建成功\n");
	}
	
	vTaskDelete(NULL);							//开始任务自杀
	taskEXIT_CRITICAL();            //恢复中断与开启任务器调度
}

//显示屏任务
void LCD_Task(void *pvParameters)
{
	while(1)
	{
		if(RFID_Flag||AS608_Flag==1||ESP8266_Flag)
		{
			jiesuo();
			delay_ms(2000);
			RFID_Flag=0;
			AS608_Flag=0;
			ESP8266_Flag=0;
		}
		else
		{
			zhujiemian();
		}
		vTaskDelay(10); //延时10ms，也就是10个时钟节拍
	}
}

//刷卡任务
void RFID_Task(void *pvParameters)
{
	int i;
	while(1)
	{
		RFID_Flag=RC522_Handel();
		if(RFID_Flag==1)
		{
			BEEP = 1;
			delay_ms(500);
			BEEP = 0;
			printf("卡号识别成功\n");
			xEventGroupSetBits(xEventHandle,EventBit0);
			err=0;
		}
		else
		{
			printf("卡号识别失败\n");
			for(i=0;i<3;i++)
			{
				BEEP = 1;
				delay_ms(500);
				BEEP = 0;
				delay_ms(500);
			}
			err++;
		}
		if(err>=3)
		{
			vTaskSuspend(xLEDTaskHandle);
			printf("错误=%d,非法入侵，智能锁停用1\n",err);
			vTaskSuspend(NULL);
		}
		vTaskDelay(10); //延时10ms，也就是10个时钟节拍
	}
}

//指纹任务
void AS608_Task(void *pvParameters)
{
	int i;
	while(1)
	{
		if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
		{
			AS608_Flag=press_FR();
		  if(AS608_Flag==1)
			{
				BEEP=1;
				delay_ms(500);
				BEEP=0;
				xEventGroupSetBits(xEventHandle,EventBit1);
				printf("指纹识别成功");
				err=0;
			}
			else
			{
				for(i=0;i<3;i++)
				{
					BEEP = 1;
					delay_ms(500);
					BEEP = 0;
					delay_ms(500);
				}
			  printf("指纹识别失败");
				err++;
			} 
		}
		if(err>=3)
		{
			vTaskSuspend(xLEDTaskHandle);
			printf("错误=%d，非法入侵，智能锁停用2\n",err);
			vTaskSuspend(NULL);
		}
		
		vTaskDelay(10); //延时10ms，也就是10个时钟节拍
	}
}

//管理员任务
void Admin_Task(void *pvParameters)
{
	vTaskSuspend(xAS608TaskHandle);
	vTaskSuspend(xLcdTaskHandle);
	u8 key_num;
	OLED_Clear();
	while(key_num!=3)
	{
		OLED_ShowString(40,0,"Admin",16);
		OLED_ShowString(0,2,"K1:Add FR",16);
		OLED_ShowString(0,4,"K2:Delete FR",16);
		OLED_ShowString(0,6,"K3:EXIT",16);
		key_num=KEY_Scan(0);
		//printf("KEY1=%d,KEY2=%d\n",(u8)KEY1,(u8)KEY2);
		if(key_num==1)
		{
			OLED_Clear();
			Add_FR();
		}
		else if(key_num==2)
		{
			OLED_Clear();
			Del_FR();
		}
	}
	OLED_Clear();
	vTaskResume(xAS608TaskHandle);
	vTaskResume(xLcdTaskHandle);
	vTaskDelete(NULL);
}

//ESP8266任务
void ESP8266_Task(void *pvParameters)
{
	BaseType_t xReturn;
	u8  ensure;
	int i,j=0;
	char guolv[9];
	while(1)
	{
		
	  if(USART2_RX_STA)
		{
			for(i=0;i<8;i++)
			{
				guolv[i]=USART2_RX_BUF[i];
			}
			printf("%s\n",guolv);
			//过滤垃圾信息
			if((strstr((const char*)guolv,"+STA_CON"))||(strstr((const char*)guolv,"0,CONNEC"))||(strstr((const char*)guolv,"+DIST_ST"))||(strstr((const char*)guolv,"0,CLOSED"))||(strstr((const char*)guolv,"+STA_DIS")))
			{
				printf("垃圾信息\n");
			}
			//输入正确密码开锁
			else if(strstr((const char*)USART2_RX_BUF,"on"))
			{
				 ESP8266_Flag=1;
				 BEEP=1;
				 delay_ms(500);
				 BEEP=0;
				 err=0;
				 printf("密码正确\r\n");
				 vTaskResume(xLEDTaskHandle);
				 vTaskResume(xRFIDTaskHandle);
				 vTaskResume(xAS608TaskHandle);
				 xEventGroupSetBits(xEventHandle,EventBit2);
				 memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
			}				
			//管理员模式，对指纹库进行操作
			else if(strstr((const char*)USART2_RX_BUF,"admin"))
			{
				taskENTER_CRITICAL();					//停止中断与任务器调度
				
				xReturn = xTaskCreate(Admin_Task,"Admin_Task",Admin_TASK_SIZE,NULL,Admin_TASK_PRIO,&xAdminTaskHandle);
				if(xReturn == pdPASS)
				{
					printf("Admin任务创建成功\n");
				}
				taskEXIT_CRITICAL();            //恢复中断与开启任务器调度
			}
			else if(strstr((const char*)USART2_RX_BUF,"super"))
			{
				vTaskSuspend(xLcdTaskHandle);
				vTaskSuspend(xAS608TaskHandle);
				ensure = PS_Empty(); //清空指纹库
				if(ensure == 0)
				{
					OLED_Clear();
					delay_ms(50);
					OLED_ShowString(20,3,"FR is empty",16);
				}
				else
					printf("%s",(u8*)EnsureMessage(ensure));
				delay_ms(1000);
				OLED_Clear();
				vTaskResume(xLcdTaskHandle);
				vTaskResume(xAS608TaskHandle);
			}
			//密码错误处理
			else
			{
				ESP8266_Flag=0;
			  printf("密码错误\r\n");
				memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
				for(i=0;i<3;i++)
				{
					BEEP = 1;
					delay_ms(500);
					BEEP = 0;
					delay_ms(500);
				}
			}				
			USART2_RX_STA=0;
		}
		vTaskDelay(10);	
	}
}


//灯闪任务
void LED_Task(void *pvParameters)
{
	while(1)
	{
		xEventGroupWaitBits(xEventHandle,EventBitAll,pdTRUE,pdFALSE,portMAX_DELAY); //等待的事件有一个为1即可，并且接收后清除标志位
		printf("成功接收事件\n");
		PBout(9)=1;
		delay_ms(2000);
		PBout(9)=0;
		vTaskDelay(10); //延时10ms，也就是10个时钟节拍
	}
}

/***********************************************************************
  * @ 函数名  ： hardware_init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
static void hardware_init()
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	delay_init(72);
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
	usart1_init(115200);
	usart2_init(115200);
	esp8266_start_trans();
	LED_Init();
	KEY_Init();
	RC522_Init();
	BEEP_Init();
	usart3_init(57600);
	PS_StaGPIO_Init();
	AS608_Init();
}

int main(void)
{
	hardware_init();
	OLED_Init();
	OLED_Clear();
	/*创建开始任务*/
	xTaskCreate(Start_Task,"Start_Task",START_TASK_SIZE,NULL,START_TASK_PRIO,&xStartTaskHandler);
	
	
	vTaskStartScheduler(); //开启任务调度
	
	return 0;
}


