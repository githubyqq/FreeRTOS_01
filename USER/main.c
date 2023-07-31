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
//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_TASK_SIZE 		512  
//������
TaskHandle_t xStartTaskHandler;
//������
void Start_Task(void *pvParameters);

//�������ȼ�
#define LCD_TASK_PRIO		3
//�����ջ��С	
#define LCD_TASK_SIZE 		512
//������
TaskHandle_t xLcdTaskHandle;
//������
void LCD_Task(void *pvParameters);

//�������ȼ�
#define RFID_TASK_PRIO		3
//�����ջ��С	
#define RFID_TASK_SIZE 		512  
//������
TaskHandle_t xRFIDTaskHandle;
//������
void RFID_Task(void *pvParameters);

//�������ȼ�
#define AS608_TASK_PRIO		3
//�����ջ��С	
#define AS608_TASK_SIZE 		512  
//������
TaskHandle_t xAS608TaskHandle;
//������
void AS608_Task(void *pvParameters);

//�������ȼ�
#define ESP8266_TASK_PRIO		3
//�����ջ��С	
#define ESP8266_TASK_SIZE 		512
//������
TaskHandle_t xESP8266TaskHandle;
//������
void ESP8266_Task(void *pvParameters);

//�������ȼ�

#define Admin_TASK_PRIO		5
//�����ջ��С	
#define Admin_TASK_SIZE 		512
//������
TaskHandle_t xAdminTaskHandle;
//������
void Admin_Task(void *pvParameters);

//�������ȼ�
#define LED_TASK_PRIO		4
//�����ջ��С	
#define LED_TASK_SIZE 		512  
//������
TaskHandle_t xLEDTaskHandle;
//������
void LED_Task(void *pvParameters);




void Start_Task(void *pvParameters)
{
	BaseType_t xReturn;
	taskENTER_CRITICAL();           //ֹͣ�ж�������������
	
	xEventHandle = xEventGroupCreate();
	if(xEventHandle != NULL)
	{
		printf("�¼��鴴���ɹ�\n");
	}
	
	xReturn = xTaskCreate(LCD_Task,"LCD_Task",LCD_TASK_SIZE,NULL,LCD_TASK_PRIO,&xLcdTaskHandle);
	if(xReturn == pdPASS)
	{
		printf("LCD���񴴽��ɹ�\n");
	}
	
	xReturn = xTaskCreate(RFID_Task,"RFID_Task",RFID_TASK_SIZE,NULL,RFID_TASK_PRIO,&xRFIDTaskHandle);
	if(xReturn == pdPASS)
	{
		printf("RFID���񴴽��ɹ�\n");
	}
	
	xReturn = xTaskCreate(AS608_Task,"AS608_Task",AS608_TASK_SIZE,NULL,AS608_TASK_PRIO,&xAS608TaskHandle);
	if(xReturn == pdPASS)
	{
		printf("AS608���񴴽��ɹ�\n");
	}
	
	xReturn = xTaskCreate(ESP8266_Task,"ESP8266_Task",ESP8266_TASK_SIZE,NULL,ESP8266_TASK_PRIO,&xESP8266TaskHandle);
	if(xReturn == pdPASS)
	{
		printf("ESP8266���񴴽��ɹ�\n");
	}
	
	xReturn = xTaskCreate(LED_Task,"LED_Task",LED_TASK_SIZE,NULL,LED_TASK_PRIO,&xLEDTaskHandle);
	if(xReturn == pdPASS)
	{
		printf("LED���񴴽��ɹ�\n");
	}
	
	vTaskDelete(NULL);							//��ʼ������ɱ
	taskEXIT_CRITICAL();            //�ָ��ж��뿪������������
}

//��ʾ������
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
		vTaskDelay(10); //��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}
}

//ˢ������
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
			printf("����ʶ��ɹ�\n");
			xEventGroupSetBits(xEventHandle,EventBit0);
			err=0;
		}
		else
		{
			printf("����ʶ��ʧ��\n");
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
			printf("����=%d,�Ƿ����֣�������ͣ��1\n",err);
			vTaskSuspend(NULL);
		}
		vTaskDelay(10); //��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}
}

//ָ������
void AS608_Task(void *pvParameters)
{
	int i;
	while(1)
	{
		if(PS_Sta)	 //���PS_Sta״̬���������ָ����
		{
			AS608_Flag=press_FR();
		  if(AS608_Flag==1)
			{
				BEEP=1;
				delay_ms(500);
				BEEP=0;
				xEventGroupSetBits(xEventHandle,EventBit1);
				printf("ָ��ʶ��ɹ�");
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
			  printf("ָ��ʶ��ʧ��");
				err++;
			} 
		}
		if(err>=3)
		{
			vTaskSuspend(xLEDTaskHandle);
			printf("����=%d���Ƿ����֣�������ͣ��2\n",err);
			vTaskSuspend(NULL);
		}
		
		vTaskDelay(10); //��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}
}

//����Ա����
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

//ESP8266����
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
			//����������Ϣ
			if((strstr((const char*)guolv,"+STA_CON"))||(strstr((const char*)guolv,"0,CONNEC"))||(strstr((const char*)guolv,"+DIST_ST"))||(strstr((const char*)guolv,"0,CLOSED"))||(strstr((const char*)guolv,"+STA_DIS")))
			{
				printf("������Ϣ\n");
			}
			//������ȷ���뿪��
			else if(strstr((const char*)USART2_RX_BUF,"on"))
			{
				 ESP8266_Flag=1;
				 BEEP=1;
				 delay_ms(500);
				 BEEP=0;
				 err=0;
				 printf("������ȷ\r\n");
				 vTaskResume(xLEDTaskHandle);
				 vTaskResume(xRFIDTaskHandle);
				 vTaskResume(xAS608TaskHandle);
				 xEventGroupSetBits(xEventHandle,EventBit2);
				 memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
			}				
			//����Աģʽ����ָ�ƿ���в���
			else if(strstr((const char*)USART2_RX_BUF,"admin"))
			{
				taskENTER_CRITICAL();					//ֹͣ�ж�������������
				
				xReturn = xTaskCreate(Admin_Task,"Admin_Task",Admin_TASK_SIZE,NULL,Admin_TASK_PRIO,&xAdminTaskHandle);
				if(xReturn == pdPASS)
				{
					printf("Admin���񴴽��ɹ�\n");
				}
				taskEXIT_CRITICAL();            //�ָ��ж��뿪������������
			}
			else if(strstr((const char*)USART2_RX_BUF,"super"))
			{
				vTaskSuspend(xLcdTaskHandle);
				vTaskSuspend(xAS608TaskHandle);
				ensure = PS_Empty(); //���ָ�ƿ�
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
			//���������
			else
			{
				ESP8266_Flag=0;
			  printf("�������\r\n");
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


//��������
void LED_Task(void *pvParameters)
{
	while(1)
	{
		xEventGroupWaitBits(xEventHandle,EventBitAll,pdTRUE,pdFALSE,portMAX_DELAY); //�ȴ����¼���һ��Ϊ1���ɣ����ҽ��պ������־λ
		printf("�ɹ������¼�\n");
		PBout(9)=1;
		delay_ms(2000);
		PBout(9)=0;
		vTaskDelay(10); //��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}
}

/***********************************************************************
  * @ ������  �� hardware_init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void hardware_init()
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
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
	/*������ʼ����*/
	xTaskCreate(Start_Task,"Start_Task",START_TASK_SIZE,NULL,START_TASK_PRIO,&xStartTaskHandler);
	
	
	vTaskStartScheduler(); //�����������
	
	return 0;
}


