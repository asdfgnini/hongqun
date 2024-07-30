

#include "bsp.h"



/* LED口对应的RCC时钟 */
#define LED_GPIO_CLK_ALLENABLE() {	\
		__HAL_RCC_GPIOD_CLK_ENABLE();	\
	};

#define GPIO_PORT_LED_1  GPIOD
#define GPIO_PIN_LED_1	GPIO_PIN_12
	


void bsp_InitLed(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* 打开GPIO时钟 */
	LED_GPIO_CLK_ALLENABLE();
	
	/*
		配置所有的LED指示灯GPIO为推挽输出模式
		由于将GPIO设置为输出时，GPIO输出寄存器的值缺省是0，因此会驱动LED点亮.
		这是我不希望的，因此在改变GPIO为输出前，先关闭LED指示灯
	*/
	bsp_LedOff(1);


	/* 配置LED */
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* 设置推挽输出 */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* 上下拉电阻不使能 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO速度等级 */

	GPIO_InitStruct.Pin = GPIO_PIN_LED_1;
	HAL_GPIO_Init(GPIO_PORT_LED_1, &GPIO_InitStruct);

}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedOn
*	功能说明: 点亮指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{
//	_no--;

	if (_no == 1)
	{
		GPIO_PORT_LED_1->BSRR = GPIO_PIN_LED_1;
	}

}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedOff
*	功能说明: 熄灭指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no)
{
//	_no--;

	if (_no == 1)
	{
		GPIO_PORT_LED_1->BSRR = (uint32_t)GPIO_PIN_LED_1 << 16U;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedToggle
*	功能说明: 翻转指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{
	if (_no == 1)
	{
		GPIO_PORT_LED_1->ODR ^= GPIO_PIN_LED_1;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_IsLedOn
*	功能说明: 判断LED指示灯是否已经点亮。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 1表示已经点亮，0表示未点亮
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{
	if (_no == 1)
	{
		if ((GPIO_PORT_LED_1->ODR & GPIO_PIN_LED_1) == 1)
		{
			return 1;
		}
		return 0;
	}


	return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
