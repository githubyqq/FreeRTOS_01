#include "exti.h"
#include "led.h"
#include "key.h"
#include "usart.h"

//外部中断0服务程序
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

    KEY_Init();	 //	按键端口初始化

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

    //GPIOB.0 中断线以及中断初始化配置   下降沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); //KEY0

  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;									//0号中断线
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//中断事件
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;									//中断线使能
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    //GPIOB.1	  中断线以及中断初始化配置 下降沿触发 //KEY1
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);	//KEY1
  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;									//1号中断线
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//中断事件
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;									////中断线使能
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能按键KEY0所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键KEY1所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 
}


 
