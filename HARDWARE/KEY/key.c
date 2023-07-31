#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"


//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTE时钟
	
	
	//初始化 KEY1-->GPIOA.0	  上拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认上拉  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB.0

	
	//初始化 KEY2-->GPIOA.1   上拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB.1
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTA,PORTE时钟
	
	//初始化 KEY3-->GPIOB.1   上拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//KEY3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.1
	
	//初始化 KEY4-->GPIOB.3   上拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//KEY4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.3
	
	//初始化 KEY5-->GPIOB.8   上拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;//KEY5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.8
	

}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//注意此函数有响应优先级,KEY0>KEY1!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==1||KEY5==0))
	{
		delay_ms(40);//去抖动 
		key_up=0;
		if(KEY1==0)return 1;
		else if(KEY2==0)return 2;
		else if(KEY3==0)return 3;
		else if(KEY4==1)return 4;
		else if(KEY5==0)return 5;
	}
	else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==0&&KEY5==1)
		key_up=1; 	    
 	return 0;// 无按键按下
}