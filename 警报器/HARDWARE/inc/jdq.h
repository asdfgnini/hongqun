/*-------------------------------------------------*/
/*            ModuleStar STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ʵ��LED���ܵ�ͷ�ļ�                */
/*                                                 */
/*-------------------------------------------------*/

#ifndef _JDQ_H__
#define _JDQ_H_

#define JDQ_ENABLE_OUT(x)    GPIO_WriteBit(GPIOB, GPIO_Pin_13,  (BitAction)x)  //����PB1 �ĵ�ƽ
#define JDQ_ENABLE_LOW       GPIO_ResetBits(GPIOB, GPIO_Pin_13)         //ʹ�����
#define JDQ_ENABLE_HIGH      GPIO_SetBits(GPIOB, GPIO_Pin_13)           //�����

#define JDQ1_OUT(x)   GPIO_WriteBit(GPIOB, GPIO_Pin_4, (BitAction)x)  //����PB10 �ĵ�ƽ
#define JDQ2_OUT(x)   GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)x)  //����PB11 �ĵ�ƽ

#define JDQ1_IN_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_4) //PB10 ��ȡ��ƽ״̬�������ж��ǵ�������Ϩ��
#define JDQ2_IN_STA   GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_12) //PB11 ��ȡ��ƽ״̬�������ж��ǵ�������Ϩ��

#define JDQ1_OFF       GPIO_ResetBits(GPIOB, GPIO_Pin_4)         //������������PB1��ƽ������
#define JDQ1_ON      GPIO_SetBits(GPIOB, GPIO_Pin_4)           //������������PB1��ƽ��Ϩ��
#define JDQ2_OFF       GPIO_ResetBits(GPIOA, GPIO_Pin_12)         //������������PA5��ƽ������
#define JDQ2_ON      GPIO_SetBits(GPIOA, GPIO_Pin_12)           //������������PA5��ƽ��Ϩ��

void JDQ_ENABLE_Init(void);
void JDQ_Init(void);               //��ʼ��	
void JDQ_AllOn(void);              //��������JDQ
void JDQ_AllOff(void);             //Ϩ������JDQ
void JDQ_SetState(unsigned char state);

#endif
