

#include "bsp.h"


/* LED口对应的RCC时钟 */
#define LED_GPIO_CLK_ALLENABLE() {	\
		__HAL_RCC_GPIOA_CLK_ENABLE();	\
		__HAL_RCC_GPIOB_CLK_ENABLE();	\
	};

	
#define GPIO_PORT_LED1  GPIOA
#define GPIO_PIN_LED1	GPIO_PIN_3

#define GPIO_PORT_LED2  GPIOB
#define GPIO_PIN_LED2	GPIO_PIN_0

#define GPIO_PORT_LED3  GPIOB
#define GPIO_PIN_LED3	GPIO_PIN_1


/*
*********************************************************************************************************
*	函 数 名: bsp_InitLed
*	功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* 打开GPIO时钟 */
	LED_GPIO_CLK_ALLENABLE();
	
	bsp_LedOff(1);
	bsp_LedOff(2);
	bsp_LedOff(3);

	/* 配置LED */
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		
	GPIO_InitStruct.Pull = GPIO_NOPULL;                 
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		

	GPIO_InitStruct.Pin = GPIO_PIN_LED1;
	HAL_GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_LED2;
	HAL_GPIO_Init(GPIO_PORT_LED2, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_LED3;
	HAL_GPIO_Init(GPIO_PORT_LED3, &GPIO_InitStruct);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedOn
*	功能说明: 点亮指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/

void bsp_LedOff(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		HAL_GPIO_WritePin(GPIO_PORT_LED1,GPIO_PIN_LED1,GPIO_PIN_SET);
	}
	else if (_no == 1)
	{
		HAL_GPIO_WritePin(GPIO_PORT_LED2,GPIO_PIN_LED2,GPIO_PIN_SET);
	}
	else if (_no == 2)
	{
		HAL_GPIO_WritePin(GPIO_PORT_LED3,GPIO_PIN_LED3,GPIO_PIN_SET);
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
void bsp_LedOn(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		HAL_GPIO_WritePin(GPIO_PORT_LED1,GPIO_PIN_LED1,GPIO_PIN_RESET);
	}
	else if (_no == 1)
	{
		HAL_GPIO_WritePin(GPIO_PORT_LED1,GPIO_PIN_LED1,GPIO_PIN_RESET);
	}
	else if (_no == 2)
	{
		HAL_GPIO_WritePin(GPIO_PORT_LED1,GPIO_PIN_LED1,GPIO_PIN_RESET);
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
		HAL_GPIO_TogglePin(GPIO_PORT_LED1,GPIO_PIN_LED1);
	}
	else if (_no == 2)
	{
		HAL_GPIO_TogglePin(GPIO_PORT_LED2,GPIO_PIN_LED2);
	}
	else if (_no == 3)
	{
		HAL_GPIO_TogglePin(GPIO_PORT_LED3,GPIO_PIN_LED3);
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
		if ((GPIO_PORT_LED1->ODR & GPIO_PIN_LED1) == 1)
		{
			return 1;
		}
		return 0;
	}
	if (_no == 2)
	{
		if ((GPIO_PORT_LED2->ODR & GPIO_PIN_LED2) == 1)
		{
			return 1;
		}
		return 0;
	}
	if (_no == 3)
	{
		if ((GPIO_PORT_LED3->ODR & GPIO_PIN_LED3) == 1)
		{
			return 1;
		}
		return 0;
	}


	return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
