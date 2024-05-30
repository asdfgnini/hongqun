/*-------------------------------------------------*/
/*            ModuleStar STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*              实现LED功能的源文件                */
/*                                                 */
/*-------------------------------------------------*/
#include "stm32f10x.h"  //包含需要的头文件
#include "jdq.h"        //包含需要的头文件

void JDQ_ENABLE_Init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  //PB13
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	JDQ_ENABLE_LOW;
}


/*-------------------------------------------------*/
/*函数名：初始化LED函数                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void JDQ_Init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     //定义一个设置GPIO的变量

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);    //开启AFIO复用功能
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	// 改变指定管脚的映射 GPIO_Remap_SWJ_JTAGDisable ，JTAG-DP 禁用 + SW-DP 使能

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);                                //使能GPIOB端口时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);                                //使能GPIOB端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  //准备设置PB8 PB9 PB10 PB11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     //IO速率50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                               //推免输出方式
	GPIO_Init(GPIOB, &GPIO_InitStructure);            		                               //设置PB8 PB9 PB10 PB11
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  //准备设置PB8 PB9 PB10 PB11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     //IO速率50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                               //推免输出方式
	GPIO_Init(GPIOA, &GPIO_InitStructure);            		                               //设置PB8 PB9 PB10 PB11   		                               //设置PB8 PB9 PB10 PB11
}

/*-------------------------------------------------*/
/*函数名：点亮所有LED                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void JDQ_AllOn(void)
{
	JDQ1_ON;
	JDQ2_ON;
}

/*-------------------------------------------------*/
/*函数名：熄灭所有LED                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void JDQ_AllOff(void)
{
	JDQ1_OFF;
	JDQ2_OFF;
}

void JDQ_SetState(unsigned char state)
{
	if (state & 0x01) {
		JDQ1_ON;		
	}else{
		JDQ1_OFF;	
	}

	if (state & 0x02) {
		JDQ2_ON;		
	}else{
		JDQ2_OFF;	
	}
}

