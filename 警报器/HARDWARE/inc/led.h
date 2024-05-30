

#ifndef __LED_H
#define __LED_H

#define STA_OUT(x)   GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)x)  //设置PA1 的电平，可以点亮熄灭LED

#define STA_IN_STA   GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1) //PA1 控制LED3,读取电平状态，可以判断LED是点亮还是熄灭

#define STA_ON       GPIO_ResetBits(GPIOA, GPIO_Pin_1)        //共阳极，拉低PAA1
#define STA_OFF      GPIO_SetBits(GPIOA, GPIO_Pin_1)          //共阳极，拉高PA1

void LED_LocationStaInit(void);               //初始化	
void LED_LocationStaOn(void);              //点亮所有LED
void LED_LocationStaOff(void);             //熄灭所有LED

//网口
#define STA2_OUT(x)   GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)x)  //设置PB10 的电平，可以点亮熄灭LED

#define STA2_IN_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10) //PB10 控制LED3,读取电平状态，可以判断LED是点亮还是熄灭

#define STA2_ON       GPIO_ResetBits(GPIOB, GPIO_Pin_10)        //共阳极，拉低PB10
#define STA2_OFF      GPIO_SetBits(GPIOB, GPIO_Pin_10)          //共阳极，拉高PB10

void LED2_LocationStaInit(void);               //初始化	
void LED2_LocationStaOn(void);              //点亮所有LED
void LED2_LocationStaOff(void);             //熄灭所有LED

#endif
