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
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
	uart_init(115200);
	LED_Init();
}


int main(void)
{
	hardware_init();
	
	xTaskCreate(TaskFunction1,"Task1",512,NULL,1,NULL);

	vTaskStartScheduler(); //开启任务调度
	
	return 0;
}

