#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK精英STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


#define KEY1 PAin(0)   	//PA0
#define KEY2 PAin(1)	//PA1
#define KEY3 PBin(1)   	//PB1
#define KEY4 PBin(3)   	//PB3
#define KEY5 PBin(8)   	//PB8


//#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)//读取按键0
//#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)//读取按键1

 

//#define KEY0_PRES 	1	//KEY0按下
//#define KEY1_PRES	  2	//KEY1按下


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数					    
#endif
