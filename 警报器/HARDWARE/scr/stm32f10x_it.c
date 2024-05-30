/*-------------------------------------------------*/
/*            ModuleStar STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*          实现各种中断服务函数的源文件           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"     //包含需要的头文件
#include "stm32f10x_it.h"  //包含需要的头文件
#include "main.h"          //包含需要的头文件
#include "delay.h"         //包含需要的头文件
#include "usart1.h"        //包含需要的头文件
#include "usart3.h"        //包含需要的头文件
#include "timer4.h"        //包含需要的头文件
#include "cat1.h"          //包含需要的头文件
#include "timer3.h"        //包含需要的头文件
#include "led.h"           //包含需要的头文件
#include "mqtt.h"          //包含需要的头文件
#include "iwdg.h"

/*-------------------------------------------------*/
/*函数名：串口3接收中断函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART3_IRQHandler(void)   
{                      
	if((USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)){                              //如果USART_IT_RXNE标志置位，表示有数据到了 进入if分支
		if(USART3->DR){                                 								  //处于指令配置状态时，非零值才保存到缓冲区	
			Usart3_RxBuff[Usart3_RxCounter]=USART3->DR;                                   //保存到缓冲区	
			Usart3_RxCounter ++;                                                          //每接收1个字节的数据，Usart3_RxCounter加1，表示接收的数据总量+1 
		}		
	}	
	if((USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)){                              //如USART_IT_IDLE标志置位，表示空闲中断 进入if分支
		Usart3_RxCounter = USART3->SR;                                                    //清除USART_IT_IDLE标志  步骤1   
		Usart3_RxCounter = USART3->DR;                                                    //清除USART_IT_IDLE标志  步骤2
		DMA_Cmd(DMA1_Channel3,DISABLE);                                                   //关闭DMA
		Usart3_RxCounter = USART3_RXBUFF_SIZE -  DMA_GetCurrDataCounter(DMA1_Channel3);   //获取串口接收的数据量
		memcpy(&MQTT_RxDataInPtr[2],Usart3_RxBuff,Usart3_RxCounter);                      //拷贝数据到接收缓冲区
		MQTT_RxDataInPtr[0] = Usart3_RxCounter/256;                                       //记录数据长度高字节
		MQTT_RxDataInPtr[1] = Usart3_RxCounter%256;                                       //记录数据长度低字节
		MQTT_RxDataInPtr+=RBUFF_UNIT;                                                     //指针下移
		if(MQTT_RxDataInPtr==MQTT_RxDataEndPtr)                                           //如果指针到缓冲区尾部了
			MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                                         //指针归位到缓冲区开头
		Usart3_RxCounter = 0;                                                             //串口3接收数据量变量清零				
		DMA1_Channel3->CNDTR=USART3_RXBUFF_SIZE;                                          //重新设置接收数据个数           
		DMA_Cmd(DMA1_Channel3,ENABLE);                                                    //开启DMA
		TIM_SetCounter(TIM3,0);                                                           //清除定时器3 计数值
	}
}  
/*-------------------------------------------------*/
/*函数名：DMA1通道2中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DMA1_Channel2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC2) != RESET){         //如果DMA1_IT_TC2置位，表示DMA1通道7发送完成中断，进入if	
		DMA_ClearITPendingBit(DMA1_IT_TC2);            //清除DMA1通道2 发送完成中断标志
		DMA_Cmd(DMA1_Channel2,DISABLE);                //关闭DMA1通道2
		DMA_flag = 0;                                  //标志位清除 空闲状态
	}
}

/* 函数名：定时器3中断服务函数 // timer3 中断时间可变*/
extern char g_connectFlag;      //外部变量声明，同服务器连接状态  0：还没有连接服务器  1：连接上服务器了
extern char g_subcribePackFlag;   //订阅报文状态      1：订阅报文成功
extern unsigned char resetCnt_x30s;
extern unsigned int longTimeReset_x30s;
extern unsigned int g_timeJiffies;

#define MAX_JDQ_VALID_NUM 4
void TIM2_IRQHandler(void)
{
	static unsigned char feedDogCnt = 0;
	static unsigned char loopTo30s=0;

	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		g_timeJiffies++;
		feedDogCnt++;
		if (feedDogCnt % 2) {
			IWDG_FeedDog();
		}
	
		loopTo30s++;
		if (loopTo30s >= 59) { // 大约30s
			loopTo30s = 0;
			if (g_subcribePackFlag == S_TOPIC_NUM) {
				MQTT_PingREQ(); //添加Ping报文到发送缓冲区 //Ping_flag自增1，表示又发送了一次ping，期待服务器的回复
			}
			resetCnt_x30s++;
			longTimeReset_x30s++;			
		}

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);      //清除TIM3溢出中断标志 	
	}
}
/*-------------------------------------------------*/
/*函数名：不可屏蔽中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：硬件出错后进入的中断处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：内存管理中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void MemManage_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：预取指失败，存储器访问失败中断处理函数   */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void BusFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：未定义的指令或非法状态处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void UsageFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：软中断，SWI 指令调用的处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SVC_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*函数名：调试监控器处理函数                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DebugMon_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*函数名：可挂起的系统服务处理函数                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void PendSV_Handler(void)
{
}

/*-------------------------------------------------*/
/*函数名：SysTic系统嘀嗒定时器处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
//void SysTick_Handler(void)
//{
// 
//}
