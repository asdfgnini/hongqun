

#ifndef __LED_H
#define __LED_H

#define STA_OUT(x)   GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)x)  //����PA1 �ĵ�ƽ�����Ե���Ϩ��LED

#define STA_IN_STA   GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1) //PA1 ����LED3,��ȡ��ƽ״̬�������ж�LED�ǵ�������Ϩ��

#define STA_ON       GPIO_ResetBits(GPIOA, GPIO_Pin_1)        //������������PAA1
#define STA_OFF      GPIO_SetBits(GPIOA, GPIO_Pin_1)          //������������PA1

void LED_LocationStaInit(void);               //��ʼ��	
void LED_LocationStaOn(void);              //��������LED
void LED_LocationStaOff(void);             //Ϩ������LED

//����
#define STA2_OUT(x)   GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)x)  //����PB10 �ĵ�ƽ�����Ե���Ϩ��LED

#define STA2_IN_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10) //PB10 ����LED3,��ȡ��ƽ״̬�������ж�LED�ǵ�������Ϩ��

#define STA2_ON       GPIO_ResetBits(GPIOB, GPIO_Pin_10)        //������������PB10
#define STA2_OFF      GPIO_SetBits(GPIOB, GPIO_Pin_10)          //������������PB10

void LED2_LocationStaInit(void);               //��ʼ��	
void LED2_LocationStaOn(void);              //��������LED
void LED2_LocationStaOff(void);             //Ϩ������LED

#endif
