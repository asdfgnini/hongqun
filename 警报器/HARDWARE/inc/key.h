/*-------------------------------------------------*/
/*            ModuleStar STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��ɨ�谴�����ܵ�ͷ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/
#ifndef __KEY_H
#define __KEY_H	

#define KEY1_IN_STA  GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)
#define KEY2_IN_STA  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)

void KEY_Init(void);	   //������ʼ��
unsigned char KEY_Scan(void);       //����ɨ�躯��	
void KEY_Exti_Init(void);  //�����ⲿ�жϳ�ʼ������

#endif
