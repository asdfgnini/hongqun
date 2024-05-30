/*-------------------------------------------------*/
/*            ModuleStar STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ʵ��LED���ܵ�Դ�ļ�                */
/*                                                 */
/*-------------------------------------------------*/
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "jdq.h"        //������Ҫ��ͷ�ļ�

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
/*����������ʼ��LED����                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void JDQ_Init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     //����һ������GPIO�ı���

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);    //����AFIO���ù���
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	// �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_JTAGDisable ��JTAG-DP ���� + SW-DP ʹ��

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);                                //ʹ��GPIOB�˿�ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);                                //ʹ��GPIOB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  //׼������PB8 PB9 PB10 PB11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     //IO����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                               //���������ʽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);            		                               //����PB8 PB9 PB10 PB11
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  //׼������PB8 PB9 PB10 PB11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     //IO����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                               //���������ʽ
	GPIO_Init(GPIOA, &GPIO_InitStructure);            		                               //����PB8 PB9 PB10 PB11   		                               //����PB8 PB9 PB10 PB11
}

/*-------------------------------------------------*/
/*����������������LED                              */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void JDQ_AllOn(void)
{
	JDQ1_ON;
	JDQ2_ON;
}

/*-------------------------------------------------*/
/*��������Ϩ������LED                              */
/*��  ������                                       */
/*����ֵ����                                       */
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

