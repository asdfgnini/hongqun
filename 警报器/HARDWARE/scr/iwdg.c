/*-------------------------------------------------*/
/*            ModuleStar STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*            实现扫描按键功能的源文件             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "iwdg.h"

/*******************************************************************************
* 函 数 名         : IWDG_Init
* 函数功能           : IWDG初始化
* 输    入         : pre:预分频系数(0-6)
                      rlr:重装载值(12位范围0xfff)
                     独立看门狗复位时间计算公式：t=(4*2^pre*rlr)/40
* 输    出         : 无
*******************************************************************************/
void IWDG_Init(unsigned char pre,unsigned int rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //取消寄存器写保护
    IWDG_SetPrescaler(pre);//设置预分频系数 0-6
    IWDG_SetReload(rlr);//设置重装载值
    IWDG_ReloadCounter();  //重装载初值
    IWDG_Enable(); //打开独立看门狗
}

/*******************************************************************************
* 函 数 名         : IWDG_FeedDog
* 函数功能           : 喂狗
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IWDG_FeedDog(void)  //喂狗
{
    IWDG_ReloadCounter();  //重装载初值
}








