#include "sys.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
void TaskFunction1(void *pvParameters)
{
	while(1)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
		vTaskDelay(1000);
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
		vTaskDelay(1000);
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
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
	uart_init(115200);
	LED_Init();
}


int main(void)
{
	hardware_init();
	
	xTaskCreate(TaskFunction1,"Task1",512,NULL,1,NULL);

	vTaskStartScheduler(); //�����������
	
	return 0;
}

