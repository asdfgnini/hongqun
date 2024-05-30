/*-------------------------------------------------*/
/*            ModuleStar STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*              实现LED功能的头文件                */
/*                                                 */
/*-------------------------------------------------*/

#ifndef _JDQ_H__
#define _JDQ_H_

#define JDQ_ENABLE_OUT(x)    GPIO_WriteBit(GPIOB, GPIO_Pin_13,  (BitAction)x)  //设置PB1 的电平
#define JDQ_ENABLE_LOW       GPIO_ResetBits(GPIOB, GPIO_Pin_13)         //使能输出
#define JDQ_ENABLE_HIGH      GPIO_SetBits(GPIOB, GPIO_Pin_13)           //不输出

#define JDQ1_OUT(x)   GPIO_WriteBit(GPIOB, GPIO_Pin_4, (BitAction)x)  //设置PB10 的电平
#define JDQ2_OUT(x)   GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)x)  //设置PB11 的电平

#define JDQ1_IN_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_4) //PB10 读取电平状态，可以判断是点亮还是熄灭
#define JDQ2_IN_STA   GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_12) //PB11 读取电平状态，可以判断是点亮还是熄灭

#define JDQ1_OFF       GPIO_ResetBits(GPIOB, GPIO_Pin_4)         //共阳极，拉低PB1电平，点亮
#define JDQ1_ON      GPIO_SetBits(GPIOB, GPIO_Pin_4)           //共阳极，拉高PB1电平，熄灭
#define JDQ2_OFF       GPIO_ResetBits(GPIOA, GPIO_Pin_12)         //共阳极，拉低PA5电平，点亮
#define JDQ2_ON      GPIO_SetBits(GPIOA, GPIO_Pin_12)           //共阳极，拉高PA5电平，熄灭

void JDQ_ENABLE_Init(void);
void JDQ_Init(void);               //初始化	
void JDQ_AllOn(void);              //点亮所有JDQ
void JDQ_AllOff(void);             //熄灭所有JDQ
void JDQ_SetState(unsigned char state);

#endif
