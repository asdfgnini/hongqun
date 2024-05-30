/*-------------------------------------------------*/
/*            ModuleStar STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��ɨ�谴�����ܵ�Դ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "delay.h"      //������Ҫ��ͷ�ļ�
#include "key.h"      //������Ҫ��ͷ�ļ�

/*-------------------------------------------------*/
/*����������ʼ��KEY��������                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void KEY_Init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;                       //����һ������IO�Ľṹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);    //ʹ��PA�˿�ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE);    //ʹ��PA�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
} 

/*-------------------------------------------------*/
/*������������ɨ�躯��                             */
/*��  ������                                       */
/*����ֵ�����ް������£��еĻ����ĸ���������       */
/*-------------------------------------------------*/
unsigned char KEY_Scan(void)
{
	static unsigned char key1_In=0;
	static unsigned char key2_In=0;
	unsigned char keyTmp1;
	unsigned char keyTmp2;

	if(KEY1_IN_STA==0){
		Delay_Ms(100);
		if(KEY1_IN_STA==0){	
			keyTmp1 = 1;
			if (key1_In != keyTmp1) {
				key1_In = keyTmp1;
				return key1_In;
			}
			return 0xff;
		}
	} else {
		key1_In=0;
	}

	if(KEY2_IN_STA==0){
		Delay_Ms(100);
		if(KEY2_IN_STA==0){	
			keyTmp2 = 2;
			if (key2_In != keyTmp2) {
				key2_In = keyTmp2;
				return key2_In;
			}
			return 0xff;
		}
	} else {
		key2_In=0;
	}
	return 0xff;
}

/*-------------------------------------------------*/
/*�������������ⲿ�жϳ�ʼ������.                    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void KEY_Exti_Init(void)
{
	// NVIC_InitTypeDef   NVIC_InitStructure; //����һ������IO�Ľṹ��
	// EXTI_InitTypeDef   EXTI_InitStructure; //����һ�������жϵĽṹ�� 

  	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	          //ʹ��GPIO���ù���ʱ��
  	// GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);    //����KEY1-PA0�ж���
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);    //����KEY2-PA1�ж���

  	// EXTI_InitStructure.EXTI_Line = EXTI_Line0;	                  //׼�������ⲿ0�ж���
  	// EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	          //�ⲿ�ж�
  	// EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        //�����ش����ж�
  	// EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     //ʹ��
  	// EXTI_Init(&EXTI_InitStructure);	 	                          //�����ⲿ�ж�0��
	
  	// EXTI_InitStructure.EXTI_Line = EXTI_Line1;	                  //׼������KEY2-PA1�ⲿ1�ж���
  	// EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	          //�ⲿ�ж�
  	// EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;       //�½��ش����ж�
  	// EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     //ʹ��
  	// EXTI_Init(&EXTI_InitStructure);	 	                          //�����ⲿ�ж�1��

	// NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			  //׼�������ⲿ�ж�0
  	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;     //��ռ���ȼ�3 
  	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		      //�����ȼ�0
  	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			      //ʹ���ⲿ�ж�0ͨ��
  	// NVIC_Init(&NVIC_InitStructure);                               //�����ⲿ�ж�0

    // NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			  //׼�������ⲿ�ж�1
  	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;     //��ռ���ȼ�3
  	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		      //�����ȼ�1
  	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				  //ʹ���ⲿ�ж�1ͨ��
  	// NVIC_Init(&NVIC_InitStructure);                               //�����ⲿ�ж�1
}




















