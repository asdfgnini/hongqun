/*-------------------------------------------------*/
/*            ModuleStar STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*          ʵ�ָ����жϷ�������Դ�ļ�           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"     //������Ҫ��ͷ�ļ�
#include "stm32f10x_it.h"  //������Ҫ��ͷ�ļ�
#include "main.h"          //������Ҫ��ͷ�ļ�
#include "delay.h"         //������Ҫ��ͷ�ļ�
#include "usart1.h"        //������Ҫ��ͷ�ļ�
#include "usart3.h"        //������Ҫ��ͷ�ļ�
#include "timer4.h"        //������Ҫ��ͷ�ļ�
#include "cat1.h"          //������Ҫ��ͷ�ļ�
#include "timer3.h"        //������Ҫ��ͷ�ļ�
#include "led.h"           //������Ҫ��ͷ�ļ�
#include "mqtt.h"          //������Ҫ��ͷ�ļ�
#include "iwdg.h"

/*-------------------------------------------------*/
/*������������3�����жϺ���                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART3_IRQHandler(void)   
{                      
	if((USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)){                              //���USART_IT_RXNE��־��λ����ʾ�����ݵ��� ����if��֧
		if(USART3->DR){                                 								  //����ָ������״̬ʱ������ֵ�ű��浽������	
			Usart3_RxBuff[Usart3_RxCounter]=USART3->DR;                                   //���浽������	
			Usart3_RxCounter ++;                                                          //ÿ����1���ֽڵ����ݣ�Usart3_RxCounter��1����ʾ���յ���������+1 
		}		
	}	
	if((USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)){                              //��USART_IT_IDLE��־��λ����ʾ�����ж� ����if��֧
		Usart3_RxCounter = USART3->SR;                                                    //���USART_IT_IDLE��־  ����1   
		Usart3_RxCounter = USART3->DR;                                                    //���USART_IT_IDLE��־  ����2
		DMA_Cmd(DMA1_Channel3,DISABLE);                                                   //�ر�DMA
		Usart3_RxCounter = USART3_RXBUFF_SIZE -  DMA_GetCurrDataCounter(DMA1_Channel3);   //��ȡ���ڽ��յ�������
		memcpy(&MQTT_RxDataInPtr[2],Usart3_RxBuff,Usart3_RxCounter);                      //�������ݵ����ջ�����
		MQTT_RxDataInPtr[0] = Usart3_RxCounter/256;                                       //��¼���ݳ��ȸ��ֽ�
		MQTT_RxDataInPtr[1] = Usart3_RxCounter%256;                                       //��¼���ݳ��ȵ��ֽ�
		MQTT_RxDataInPtr+=RBUFF_UNIT;                                                     //ָ������
		if(MQTT_RxDataInPtr==MQTT_RxDataEndPtr)                                           //���ָ�뵽������β����
			MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                                         //ָ���λ����������ͷ
		Usart3_RxCounter = 0;                                                             //����3������������������				
		DMA1_Channel3->CNDTR=USART3_RXBUFF_SIZE;                                          //�������ý������ݸ���           
		DMA_Cmd(DMA1_Channel3,ENABLE);                                                    //����DMA
		TIM_SetCounter(TIM3,0);                                                           //�����ʱ��3 ����ֵ
	}
}  
/*-------------------------------------------------*/
/*��������DMA1ͨ��2�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DMA1_Channel2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC2) != RESET){         //���DMA1_IT_TC2��λ����ʾDMA1ͨ��7��������жϣ�����if	
		DMA_ClearITPendingBit(DMA1_IT_TC2);            //���DMA1ͨ��2 ��������жϱ�־
		DMA_Cmd(DMA1_Channel2,DISABLE);                //�ر�DMA1ͨ��2
		DMA_flag = 0;                                  //��־λ��� ����״̬
	}
}

/* ����������ʱ��3�жϷ����� // timer3 �ж�ʱ��ɱ�*/
extern char g_connectFlag;      //�ⲿ����������ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
extern char g_subcribePackFlag;   //���ı���״̬      1�����ı��ĳɹ�
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
		if (loopTo30s >= 59) { // ��Լ30s
			loopTo30s = 0;
			if (g_subcribePackFlag == S_TOPIC_NUM) {
				MQTT_PingREQ(); //���Ping���ĵ����ͻ����� //Ping_flag����1����ʾ�ַ�����һ��ping���ڴ��������Ļظ�
			}
			resetCnt_x30s++;
			longTimeReset_x30s++;			
		}

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);      //���TIM3����жϱ�־ 	
	}
}
/*-------------------------------------------------*/
/*�����������������жϴ�����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ӳ������������жϴ�����             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*���������ڴ�����жϴ�����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void MemManage_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ԥȡָʧ�ܣ��洢������ʧ���жϴ�����   */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void BusFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������δ�����ָ���Ƿ�״̬������           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void UsageFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*�����������жϣ�SWI ָ����õĴ�����           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SVC_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*�����������Լ����������                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DebugMon_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*���������ɹ����ϵͳ��������                 */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void PendSV_Handler(void)
{
}

/*-------------------------------------------------*/
/*��������SysTicϵͳ��શ�ʱ��������             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
//void SysTick_Handler(void)
//{
// 
//}
