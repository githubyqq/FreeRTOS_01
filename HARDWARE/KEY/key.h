#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK��ӢSTM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


#define KEY1 PAin(0)   	//PA0
#define KEY2 PAin(1)	//PA1
#define KEY3 PBin(1)   	//PB1
#define KEY4 PBin(3)   	//PB3
#define KEY5 PBin(8)   	//PB8


//#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)//��ȡ����0
//#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)//��ȡ����1

 

//#define KEY0_PRES 	1	//KEY0����
//#define KEY1_PRES	  2	//KEY1����


void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��					    
#endif
