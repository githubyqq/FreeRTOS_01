#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 


#define USART2_MAX_RECV_LEN		128					//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		128					//����ͻ����ֽ���


extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�


extern u16 USART2_RX_STA;   						//��������״̬

void usart2_init(u32 bound);
void u2_printf(char* fmt,...);
#endif
 