#include "sys.h"
#include "usart.h"	  
#include "stdio.h"
#include "string.h"
#include <stdarg.h>
#include "stm32f10x_tim.h"


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

UART_BUF buf_uart1;     //CH340

void UART1_send_byte(char data)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, data);
}


//bound:波特率
void usart1_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
		USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启空闲中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接收中断
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}


void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(USARTx, *str++);									//发送数据
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
    }

}

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
    va_end(ap);

    while(*pStr != 0)
    {
        USART_SendData(USARTx, *pStr++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }

}


void Uart1_SendStr(char*SendBuf)//串口1打印数据
{
	while(*SendBuf)
	{
        while((USART1->SR&0X40)==0);//等待发送完成 
        USART1->DR = (u8) *SendBuf; 
        SendBuf++;
	}
}




void USART1_IRQHandler(void)                	//串口1中断服务程序
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断，可以扩展来控制
    {
        buf_uart1.buf[buf_uart1.index++] =USART_ReceiveData(USART1);//接收模块的数据
    }
} 



