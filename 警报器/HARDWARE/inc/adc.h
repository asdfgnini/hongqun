/*-------------------------------------------------*/
/*            ModuleStar STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*              操作ADC功能的头文件                */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __ADC_H
#define __ADC_H	
							   
void Adc_Init(void); 				  //初始化ADC
int Get_Adc(int ch) ; 				  //获得ADC结果 
int Get_Adc_Average(int ch,int times);//平均多次ADC结果，提高精度

#endif 
