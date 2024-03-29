#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"	
#include "sys.h" 

#define USART3_MAX_RECV_LEN		200					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		200					//最大发送缓存字节数

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节

extern u16 USART3_RX_STA;   						//接收数据状态

void usart3_init(u32 bound);				//串口3初始化 

#endif